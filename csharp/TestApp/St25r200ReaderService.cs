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
        int BaudRate = 921600,
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
                {
                    Log?.Invoke($"ERROR: {ex.GetType().Name}: {ex.Message}");
                    if (ex.StackTrace != null)
                        Log?.Invoke($"STACK: {ex.StackTrace}");
                }

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
        var rsp = SendAndReceive(SerCommandId.RfalNfcInitializeReq, Array.Empty<byte>(), expectedRspPayloadLen: 2);
        
        if (rsp.Length < 2)
            throw new IOException($"rfalNfcInitialize: Expected 2-byte response, got {rsp.Length} bytes");
            
        ushort ret = ReadU16BE(rsp, 0);
        if (ret != 0)
            throw new IOException($"rfalNfcInitialize failed ret=0x{ret:X4} ({RfalDecoders.DescribeReturnCode(ret)})");
    }

    private void RfalNfcDiscover(byte[] discoverParams93Bytes)
    {
        var rsp = SendAndReceive(SerCommandId.RfalNfcDiscoverReq, discoverParams93Bytes, expectedRspPayloadLen: 2);
        
        if (rsp.Length < 2)
            throw new IOException($"rfalNfcDiscover: Expected 2-byte response, got {rsp.Length} bytes");
            
        ushort ret = ReadU16BE(rsp, 0);
        if (ret != 0)
            throw new IOException($"rfalNfcDiscover failed ret=0x{ret:X4} ({RfalDecoders.DescribeReturnCode(ret)})");
    }

    private uint RfalNfcGetState()
    {
        var rsp = SendAndReceive(SerCommandId.RfalNfcGetStateReq, Array.Empty<byte>(), expectedRspPayloadLen: 4);
        
        if (rsp.Length < 4)
            throw new IOException($"rfalNfcGetState: Expected 4-byte response, got {rsp.Length} bytes");
            
        return ReadU32BE(rsp, 0);
    }

    private void RfalNfcDeactivate(uint deactType)
    {
        byte[] pl = new byte[4];
        BinaryPrimitives.WriteUInt32BigEndian(pl, deactType);

        var rsp = SendAndReceive(SerCommandId.RfalNfcDeactivateReq, pl, expectedRspPayloadLen: 2);
        
        if (rsp.Length < 2)
            throw new IOException($"rfalNfcDeactivate: Expected 2-byte response, got {rsp.Length} bytes");
            
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
            {
                // NFC-V UIDs are transmitted LSB-first but displayed MSB-first, so reverse
                byte[] uidBytes = uid.ToArray();
                Array.Reverse(uidBytes);
                uids.Add(BytesToHex(uidBytes));
            }
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

    private byte[] SendAndReceive(SerCommandId requestCmdId, byte[] payload, int? expectedRspPayloadLen = null)
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

        // Send in chunks to avoid overwhelming device RX buffer
        const int chunkSize = 32;
        for (int offset = 0; offset < frame.Length; offset += chunkSize)
        {
            int bytesToSend = Math.Min(chunkSize, frame.Length - offset);
            port.Write(frame, offset, bytesToSend);
            
            // Small delay between chunks (except after last chunk)
            if (offset + bytesToSend < frame.Length)
                Thread.Sleep(5);
        }

        ushort expectedRspCmd = (ushort)((ushort)requestCmdId + 1);
        
        // Read responses, skipping any unsolicited messages (e.g., 0xF00D notifications)
        const int maxRetries = 5;
        for (int attempt = 0; attempt < maxRetries; attempt++)
        {
            var (rspCmd, rspPayload, rawFrame) = ReadFrameWithRaw(port);

            if (_opt.Debug >= DebugLevel.Frames)
                Log?.Invoke($"RX cmd=0x{rspCmd:X4} payloadLen={rspPayload.Length} raw={BytesToHex(rawFrame)}");

            // If we get the expected response, return it
            if (rspCmd == expectedRspCmd)
            {
                // Optional validation: check if we got the expected payload size
                if (expectedRspPayloadLen.HasValue && rspPayload.Length != expectedRspPayloadLen.Value)
                {
                    Log?.Invoke($"WARN: Expected payload length {expectedRspPayloadLen.Value} but got {rspPayload.Length}");
                }

                return rspPayload;
            }

            // Handle device error notifications specifically
            if (rspCmd == (ushort)SerCommandId.SysErrorRsp && rspPayload.Length >= 4)
            {
                uint errorCode = ReadU32BE(rspPayload, 0);
                string errorMsg = RfalDecoders.DescribeReturnCode((ushort)errorCode);
                Log?.Invoke($"ERROR: Device reported error 0x{errorCode:X8} ({errorMsg})");
                // Continue to see if expected response follows
            }
            else if (_opt.Debug >= DebugLevel.Errors)
            {
                // Got an unexpected message - log it and try reading the next frame
                Log?.Invoke($"WARN: Skipping unsolicited message cmdId=0x{rspCmd:X4} (expected 0x{expectedRspCmd:X4})");
            }
        }

        throw new IOException($"Did not receive expected response 0x{expectedRspCmd:X4} after {maxRetries} attempts");
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

        // IMPORTANT: The documentation says "length from Command ID onwards", which INCLUDES the command ID itself.
        // So payloadLen should be (len - 2). However, some firmware versions may have bugs.
        // The safest approach is to read the bytes as specified, but be prepared for zero-length payloads.
        int payloadLen = len - 2;
        if (payloadLen < 0)
        {
            Log?.Invoke($"WARN: Invalid payloadLen={payloadLen} (len={len}). Treating as zero.");
            payloadLen = 0;
        }
        
        // DIAGNOSTIC: Log the parsed values
        if (_opt.Debug >= DebugLevel.Frames)
            Log?.Invoke($"Parsed: len={len}, cmdId=0x{cmdId:X4}, payloadLen={payloadLen}");
        
        byte[] payload = new byte[payloadLen];
        if (payloadLen > 0)
            ReadExact(port, payload, 0, payloadLen);

        // Raw frame (for dump): [AA][len2][cmd2][payload...]
        byte[] raw = new byte[1 + 2 + 2 + payloadLen];
        raw[0] = FrameHeader;
        Buffer.BlockCopy(lenBuf, 0, raw, 1, 2);
        Buffer.BlockCopy(cmdBuf, 0, raw, 3, 2);
        if (payloadLen > 0)
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
    {
        if (buf == null) throw new ArgumentNullException(nameof(buf));
        if (ofs < 0 || ofs + 2 > buf.Length)
            throw new IndexOutOfRangeException($"ReadU16BE: Buffer has {buf.Length} bytes, but trying to read 2 bytes at offset {ofs}");
        return BinaryPrimitives.ReadUInt16BigEndian(buf.AsSpan(ofs, 2));
    }

    private static uint ReadU32BE(byte[] buf, int ofs)
    {
        if (buf == null) throw new ArgumentNullException(nameof(buf));
        if (ofs < 0 || ofs + 4 > buf.Length)
            throw new IndexOutOfRangeException($"ReadU32BE: Buffer has {buf.Length} bytes, but trying to read 4 bytes at offset {ofs}");
        return BinaryPrimitives.ReadUInt32BigEndian(buf.AsSpan(ofs, 4));
    }

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
        // Based on wire capture: ST app sends length=0xA9 (169 bytes including cmdId)
        // Payload is 169 - 2 = 167 bytes (not 149 as we thought!)
        const int totalSize = 167;
        byte[] p = new byte[totalSize];
        int o = 0;

        WriteU32BE(p, ref o, (uint)Rfal.ComplianceMode.Nfc); // compMode (4 bytes)
        WriteU16BE(p, ref o, 0x002B); // techs2Find (NFC-A + NFC-B + NFC-V + ST25TB, matching wire capture)
        WriteU16BE(p, ref o, (ushort)Rfal.NfcPollTech.None); // techs2Bail
        WriteU16BE(p, ref o, 0x00C8); // totalDuration (200 ms, matching wire capture: 0x00C8)
        p[o++] = 0x05; // devLimit (5 tags)
        WriteU32BE(p, ref o, (uint)Rfal.BitRate.Keep); // maxBR (4 bytes, 0xFF)
        WriteU32BE(p, ref o, (uint)Rfal.BitRate.Br212); // nfcfBR (4 bytes)
        o += 10; // nfcid3[10]
        o += 48; // GB[48]
        p[o++] = 0x00; // GBLen
        WriteU32BE(p, ref o, (uint)Rfal.BitRate.Br424); // ap2pBR (4 bytes)
        p[o++] = 0x00; // p2pNfcaPrio
        WriteU32BE(p, ref o, 0x00000008); // isoDepFS (4 bytes)
        p[o++] = 0x03; // nfcDepLR

        o += 17; // lmConfigPA[17]
        o += 21; // lmConfigPF[21]

        p[o++] = 0x00; // wakeupEnabled
        p[o++] = 0x00; // wakeupConfigDefault
        o += 18; // wakeupConfig[18]
        p[o++] = 0x00; // wakeupPollBefore
        WriteU16BE(p, ref o, 0x0000); // wakeupNPolls

        // We're at 149 bytes but need 167, so missing 18 bytes
        // Pad with zeros for the missing fields (likely serialization padding or additional fields)
        while (o < totalSize)
        {
            p[o++] = 0x00;
        }

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
