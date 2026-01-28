// St25r200ReaderService.cs
// Open-source RFAL serial service: NFC-V (ISO15693) presence only.
// Uses public headers from ST25R200_Eval_GUI for enums, sizes, and naming.

using System;
using System.Buffers.Binary;
using System.Collections.Generic;
using System.IO;
using System.IO.Ports;
using System.Linq;
using System.Threading;

namespace St25R200.OpenSource;

public sealed class St25r200ReaderService : IDisposable
{
    public event Action<string>? TagArrived;
    public event Action<string>? TagLeft;
    public event Action<string>? Log;

    public enum DebugLevel
    {
        None = 0,
        Errors = 1,
        Frames = 2,
        Bytes = 3,
    }

    public sealed record Options(
        string PortName,
        int BaudRate = 115200,
        int ReadTimeoutMs = 400,
        int WriteTimeoutMs = 400,
        int LoopDelayMs = 75,
        int ReconnectBackoffMs = 1000,
        int MaxBackoffMs = 15000,
        int MaxTrackedTags = 4,
        DebugLevel Debug = DebugLevel.Errors,
        string RestBaseUri = "http://192.168.1.100/",
        int RestTimeoutMs = 200
    );

    private readonly Options _opt;
    private readonly PresenceTracker _tracker;
    private readonly RestNotifier _notifier;

    public St25r200ReaderService(Options opt)
    {
        _opt = opt;
        _tracker = new PresenceTracker(opt.MaxTrackedTags);
        _notifier = new RestNotifier(opt.RestBaseUri, TimeSpan.FromMilliseconds(opt.RestTimeoutMs));
    }

    public void Start()
    {
        lock (_gate)
        {
            if (_thread != null) throw new InvalidOperationException("Already started.");
            _stopRequested = false;
            _thread = new Thread(RunLoop) { IsBackground = true, Name = "ST25R200 Reader Loop" };
            _thread.Start();
        }
    }

    public void Stop()
    {
        lock (_gate) _stopRequested = true;
        _thread?.Join(2000);
    }

    public void Dispose()
    {
        Stop();
        SafeClosePort();
        _notifier.Dispose();
    }

    private readonly object _gate = new();
    private Thread? _thread;
    private volatile bool _stopRequested;
    private SerialPort? _port;

    // Protocol
    private const byte FrameHeader = 0xAA;

    private void RunLoop()
    {
        int backoff = _opt.ReconnectBackoffMs;

        while (!_stopRequested)
        {
            try
            {
                EnsureConnected();

                RfalNfcInitialize();
                RfalNfcDiscover(BuildDiscoverParams_NfcV_Default());

                PublishPresenceDelta(new HashSet<string>(StringComparer.OrdinalIgnoreCase));
                backoff = _opt.ReconnectBackoffMs;

                while (!_stopRequested)
                {
                    uint state = RfalNfcGetState();

                    if (_opt.Debug >= DebugLevel.Frames)
                    {
                        Log?.Invoke($"State=0x{state:X8} {RfalDecoders.DescribeState(state)}");
                    }

                    if (state == (uint)Rfal.NfcState.Activated)
                    {
                        var uids = RfalNfcGetDevicesFound_ParseUidsNfcVOnly();
                        PublishPresenceDelta(uids);

                        RfalNfcDeactivate((uint)Rfal.NfcDeactivateType.Idle);
                        RfalNfcDiscover(BuildDiscoverParams_NfcV_Default());
                    }

                    Thread.Sleep(_opt.LoopDelayMs);
                }
            }
            catch (Exception ex)
            {
                if (_opt.Debug >= DebugLevel.Errors)
                    Log?.Invoke($"ERROR: {ex.GetType().Name}: {ex.Message}");

                SafeClosePort();

                Thread.Sleep(backoff);
                backoff = Math.Min(_opt.MaxBackoffMs, backoff * 2);
            }
        }
    }

    private void EnsureConnected()
    {
        if (_port is { IsOpen: true }) return;

        SafeClosePort();

        var p = new SerialPort(_opt.PortName, _opt.BaudRate)
        {
            ReadTimeout = _opt.ReadTimeoutMs,
            WriteTimeout = _opt.WriteTimeoutMs,
            DtrEnable = true,
            RtsEnable = true,
        };

        p.Open();
        _port = p;

        try { p.DiscardInBuffer(); p.DiscardOutBuffer(); } catch { }

        Log?.Invoke($"Connected: {_opt.PortName} @ {_opt.BaudRate}");
    }

    private void SafeClosePort()
    {
        try
        {
            if (_port != null)
            {
                if (_port.IsOpen) _port.Close();
                _port.Dispose();
            }
        }
        catch { }
        finally
        {
            _port = null;
        }
    }

    // ===== High-level commands =====

    private void RfalNfcInitialize()
    {
        var rsp = SendAndReceive(SerCommandId.RfalNfcInitializeReq, Array.Empty<byte>());
        ushort ret = ReadU16BE(rsp, 0);
        if (ret != 0)
            throw new IOException($"rfalNfcInitialize failed ret=0x{ret:X4} ({RfalDecoders.DescribeReturnCode(ret)})");
    }

    private void RfalNfcDiscover(byte[] discoverParams93Bytes)
    {
        var rsp = SendAndReceive(SerCommandId.RfalNfcDiscoverReq, discoverParams93Bytes);
        ushort ret = ReadU16BE(rsp, 0);
        if (ret != 0)
            throw new IOException($"rfalNfcDiscover failed ret=0x{ret:X4} ({RfalDecoders.DescribeReturnCode(ret)})");
    }

    private uint RfalNfcGetState()
    {
        var rsp = SendAndReceive(SerCommandId.RfalNfcGetStateReq, Array.Empty<byte>());
        return ReadU32BE(rsp, 0);
    }

    private void RfalNfcDeactivate(uint deactType)
    {
        byte[] pl = new byte[4];
        BinaryPrimitives.WriteUInt32BigEndian(pl, deactType);

        var rsp = SendAndReceive(SerCommandId.RfalNfcDeactivateReq, pl);
        ushort ret = ReadU16BE(rsp, 0);
        if (ret != 0)
            throw new IOException($"rfalNfcDeactivate failed ret=0x{ret:X4} ({RfalDecoders.DescribeReturnCode(ret)})");
    }

    // ===== Inventory parsing (NFC-V only) =====

    private HashSet<string> RfalNfcGetDevicesFound_ParseUidsNfcVOnly()
    {
        var rsp = SendAndReceive(SerCommandId.RfalNfcGetDevicesReq, Array.Empty<byte>());

        int ofs = 0;
        ushort ret = ReadU16BE(rsp, ofs); ofs += 2;
        if (ret != 0)
            throw new IOException($"rfalNfcGetDevicesFound failed ret=0x{ret:X4} ({RfalDecoders.DescribeReturnCode(ret)})");

        byte devCnt = rsp[ofs++];
        var uids = new HashSet<string>(StringComparer.OrdinalIgnoreCase);

        for (int i = 0; i < devCnt; i++)
        {
            uint devType = ReadU32BE(rsp, ofs); ofs += 4;

            if (_opt.Debug >= DebugLevel.Frames)
                Log?.Invoke($"Device[{i}] devType=0x{devType:X8} {RfalDecoders.DescribeDevType(devType)}");

            // Flattened layout of serFlatRfalNfcDevice (serCommonTypes.h).
            // NFC-A (rfalNfcaListenDevice): type(4) + sensRes(2) + selRes(1) + nfcId1Len(1) + nfcId1[10] + isSleep(1)
            _ = ReadU32BE(rsp, ofs); ofs += 4; // nfcaType
            ofs += 2; // sensRes
            ofs += 1; // selRes
            byte nfcId1Len = rsp[ofs++];
            if (nfcId1Len > 32) throw new IOException($"nfca nfcId1Len insane: {nfcId1Len}");
            ofs += nfcId1Len; // nfcId1
            ofs += 1; // nfca isSleep

            // NFC-B (rfalNfcbListenDevice): sensbResLen(1) + sensbRes[5] + isSleep(1)
            _ = rsp[ofs++]; // sensbResLen
            ofs += 5; // sensbRes
            ofs += 1; // nfcb isSleep

            // NFC-F (rfalNfcfListenDevice): sensfResLen(1) + sensfRes[19]
            _ = rsp[ofs++]; // sensfResLen
            ofs += 19; // sensfRes

            // NFC-V (rfalNfcvListenDevice): InvRes (resFlag+dsfid+uid+crc) + isSleep
            ofs += 1; // resFlag
            ofs += 1; // dsfid
            ReadOnlySpan<byte> uid = rsp.AsSpan(ofs, Rfal.NfcvUidLength); ofs += Rfal.NfcvUidLength;
            ofs += 2; // crc
            ofs += 1; // isSleep

            // ST25TB (rfalSt25tbListenDevice): chipID(1) + UID[8] + isDeselected(1)
            ofs += 1; // chipID
            ofs += 8; // UID
            ofs += 1; // isDeselected

            if (devType == (uint)Rfal.NfcDevType.ListenNfcv)
                uids.Add(BytesToHex(uid));
        }

        return uids;
    }

    private void PublishPresenceDelta(HashSet<string> nowPresent)
    {
        var delta = _tracker.Update(nowPresent);
        if (delta.Clipped)
        {
            Log?.Invoke($"WARN: found {delta.OriginalCount} tags but only tracking {delta.Max}");
        }

        foreach (var uid in delta.Arrived)
        {
            TagArrived?.Invoke(uid);
            _notifier.PostPlaced(uid, Log);
        }

        foreach (var uid in delta.Left)
        {
            TagLeft?.Invoke(uid);
            _notifier.PostRemoved(uid, Log);
        }
    }

    // ===== Transport =====

    private byte[] SendAndReceive(SerCommandId requestCmdId, byte[] payload)
    {
        var port = _port ?? throw new IOException("Serial port not open.");

        ushort len = checked((ushort)(2 + payload.Length));
        byte[] frame = new byte[1 + 2 + 2 + payload.Length];
        frame[0] = FrameHeader;
        WriteU16BE(frame, 1, len);
        WriteU16BE(frame, 3, (ushort)requestCmdId);
        Buffer.BlockCopy(payload, 0, frame, 5, payload.Length);

        if (_opt.Debug >= DebugLevel.Frames)
            Log?.Invoke($"TX cmd=0x{(ushort)requestCmdId:X4} len={len} bytes={frame.Length} :: {BytesToHex(frame)}");

        port.Write(frame, 0, frame.Length);

        ushort expectedRspCmd = (ushort)((ushort)requestCmdId + 1);
        var (rspCmd, rspPayload, rawFrame) = ReadFrameWithRaw(port);

        if (_opt.Debug >= DebugLevel.Frames)
            Log?.Invoke($"RX cmd=0x{rspCmd:X4} payloadLen={rspPayload.Length} raw={BytesToHex(rawFrame)}");

        if (rspCmd != expectedRspCmd)
            throw new IOException($"Unexpected rsp cmdId=0x{rspCmd:X4}, expected=0x{expectedRspCmd:X4}");

        return rspPayload;
    }

    private (ushort cmdId, byte[] payload, byte[] rawFrame) ReadFrameWithRaw(SerialPort port)
    {
        // Resync on 0xAA. If your device emits ASCII text or other frames, this makes it survivable.
        int skipped = 0;
        while (true)
        {
            int b = port.ReadByte();
            if (b < 0) throw new EndOfStreamException();

            if ((byte)b == FrameHeader) break;

            skipped++;
            if (_opt.Debug >= DebugLevel.Bytes && skipped <= 64)
                Log?.Invoke($"Resync skip byte: 0x{((byte)b):X2}");

            if (skipped > 4096)
                throw new IOException("Could not resync to 0xAA header (skipped > 4096 bytes).");
        }

        byte[] lenBuf = new byte[2];
        ReadExact(port, lenBuf, 0, 2);
        ushort len = BinaryPrimitives.ReadUInt16BigEndian(lenBuf);

        if (_opt.Debug >= DebugLevel.Bytes)
            Log?.Invoke($"Header OK. lenBytes={BytesToHex(lenBuf)} => len={len}");

        if (len < 2) throw new IOException($"Invalid len={len}");

        byte[] cmdBuf = new byte[2];
        ReadExact(port, cmdBuf, 0, 2);
        ushort cmdId = BinaryPrimitives.ReadUInt16BigEndian(cmdBuf);

        int payloadLen = len - 2;
        byte[] payload = new byte[payloadLen];
        ReadExact(port, payload, 0, payloadLen);

        // Raw frame (for dump): [AA][len2][cmd2][payload...]
        byte[] raw = new byte[1 + 2 + 2 + payloadLen];
        raw[0] = FrameHeader;
        Buffer.BlockCopy(lenBuf, 0, raw, 1, 2);
        Buffer.BlockCopy(cmdBuf, 0, raw, 3, 2);
        Buffer.BlockCopy(payload, 0, raw, 5, payloadLen);

        return (cmdId, payload, raw);
    }

    private static void ReadExact(SerialPort port, byte[] buffer, int offset, int count)
    {
        int readTotal = 0;
        while (readTotal < count)
        {
            int n = port.Read(buffer, offset + readTotal, count - readTotal);
            if (n <= 0) throw new EndOfStreamException();
            readTotal += n;
        }
    }

    // ===== Helpers =====

    private static ushort ReadU16BE(byte[] buf, int ofs)
        => BinaryPrimitives.ReadUInt16BigEndian(buf.AsSpan(ofs, 2));

    private static uint ReadU32BE(byte[] buf, int ofs)
        => BinaryPrimitives.ReadUInt32BigEndian(buf.AsSpan(ofs, 4));

    private static void WriteU16BE(byte[] buf, int ofs, ushort v)
        => BinaryPrimitives.WriteUInt16BigEndian(buf.AsSpan(ofs, 2), v);

    private static string BytesToHex(ReadOnlySpan<byte> bytes)
    {
        char[] c = new char[bytes.Length * 2];
        int k = 0;
        for (int i = 0; i < bytes.Length; i++)
        {
            byte b = bytes[i];
            c[k++] = GetHex(b >> 4);
            c[k++] = GetHex(b & 0x0F);
        }
        return new string(c);

        static char GetHex(int v) => (char)(v < 10 ? ('0' + v) : ('A' + (v - 10)));
    }

    // ===== Discover params builder =====

    private static byte[] BuildDiscoverParams_NfcV_Default()
    {
        // serRfalNfcDiscoverParam (serCommonTypes.h) serialized into 93 bytes.
        // Values follow serRfalNfcDefaultDiscParams with NFC-V polling only.
        byte[] p = new byte[93];
        int o = 0;

        WriteU32BE(p, ref o, (uint)Rfal.ComplianceMode.Nfc); // compMode
        WriteU16BE(p, ref o, (ushort)Rfal.NfcPollTech.V); // techs2Find
        WriteU16BE(p, ref o, (ushort)Rfal.NfcPollTech.None); // techs2Bail
        WriteU16BE(p, ref o, 0x00C8); // totalDuration (200 ms)
        p[o++] = 0x04; // devLimit (up to 4 tags)
        WriteU32BE(p, ref o, (uint)Rfal.BitRate.Keep); // maxBR
        WriteU32BE(p, ref o, (uint)Rfal.BitRate.Br212); // nfcfBR
        o += 10; // nfcid3[10]
        o += 48; // GB[48]
        p[o++] = 0x00; // GBLen
        WriteU32BE(p, ref o, (uint)Rfal.BitRate.Br424); // ap2pBR
        p[o++] = 0x00; // p2pNfcaPrio
        WriteU32BE(p, ref o, 0x00000008); // isoDepFS (RFAL_ISODEP_FSXI_256)
        p[o++] = 0x03; // nfcDepLR (RFAL_NFCDEP_LR_254)

        o += 17; // lmConfigPA
        o += 21; // lmConfigPF

        p[o++] = 0x00; // wakeupEnabled
        p[o++] = 0x00; // wakeupConfigDefault
        o += 18; // wakeupConfig
        p[o++] = 0x00; // wakeupPollBefore
        WriteU16BE(p, ref o, 0x0000); // wakeupNPolls

        if (o != 93) throw new InvalidOperationException($"Discover param size mismatch: {o}");
        return p;

        static void WriteU16BE(byte[] b, ref int ofs, ushort v)
        {
            BinaryPrimitives.WriteUInt16BigEndian(b.AsSpan(ofs, 2), v);
            ofs += 2;
        }

        static void WriteU32BE(byte[] b, ref int ofs, uint v)
        {
            BinaryPrimitives.WriteUInt32BigEndian(b.AsSpan(ofs, 4), v);
            ofs += 4;
        }
    }

    private static void WriteU32BE(byte[] b, ref int ofs, uint v)
    {
        BinaryPrimitives.WriteUInt32BigEndian(b.AsSpan(ofs, 4), v);
        ofs += 4;
    }

    private static void WriteU16BE(byte[] b, ref int ofs, ushort v)
    {
        BinaryPrimitives.WriteUInt16BigEndian(b.AsSpan(ofs, 2), v);
        ofs += 2;
    }
}
