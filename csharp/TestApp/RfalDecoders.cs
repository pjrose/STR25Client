using System;
using System.Collections.Generic;

namespace St25R200.OpenSource;

public static class RfalDecoders
{
    private static readonly IReadOnlyDictionary<Rfal.ReturnCode, string> ReturnCodes =
        new Dictionary<Rfal.ReturnCode, string>
        {
            [Rfal.ReturnCode.None] = "No error",
            [Rfal.ReturnCode.NoMem] = "Not enough memory",
            [Rfal.ReturnCode.Busy] = "Device/resource busy",
            [Rfal.ReturnCode.Io] = "Generic I/O error",
            [Rfal.ReturnCode.Timeout] = "Timeout",
            [Rfal.ReturnCode.Request] = "Invalid request",
            [Rfal.ReturnCode.NoMsg] = "No message",
            [Rfal.ReturnCode.Param] = "Parameter error",
            [Rfal.ReturnCode.System] = "System error",
            [Rfal.ReturnCode.Framing] = "Framing error",
            [Rfal.ReturnCode.Overrun] = "Overrun error",
            [Rfal.ReturnCode.Proto] = "Protocol error",
            [Rfal.ReturnCode.Internal] = "Internal error",
            [Rfal.ReturnCode.Again] = "Try again",
            [Rfal.ReturnCode.MemCorrupt] = "Memory corruption",
            [Rfal.ReturnCode.NotImplemented] = "Not implemented",
            [Rfal.ReturnCode.PcCorrupt] = "Program counter corruption",
            [Rfal.ReturnCode.Send] = "Send error",
            [Rfal.ReturnCode.Ignore] = "Ignored error",
            [Rfal.ReturnCode.Semantic] = "Semantic error",
            [Rfal.ReturnCode.Syntax] = "Syntax error",
            [Rfal.ReturnCode.Crc] = "CRC error",
            [Rfal.ReturnCode.NotFound] = "Transponder not found",
            [Rfal.ReturnCode.NotUnique] = "Transponder not unique",
            [Rfal.ReturnCode.NotSupported] = "Operation not supported",
            [Rfal.ReturnCode.Write] = "Write error",
            [Rfal.ReturnCode.Fifo] = "FIFO error",
            [Rfal.ReturnCode.Parity] = "Parity error",
            [Rfal.ReturnCode.Done] = "Transfer already finished",
            [Rfal.ReturnCode.RfCollision] = "RF collision",
            [Rfal.ReturnCode.HwOverrun] = "HW overrun",
            [Rfal.ReturnCode.ReleaseReq] = "Release requested",
            [Rfal.ReturnCode.SleepReq] = "Sleep requested",
            [Rfal.ReturnCode.WrongState] = "Wrong state",
            [Rfal.ReturnCode.MaxReruns] = "Max reruns",
            [Rfal.ReturnCode.Disabled] = "Disabled",
            [Rfal.ReturnCode.HwMismatch] = "HW mismatch",
            [Rfal.ReturnCode.LinkLoss] = "Link loss",
            [Rfal.ReturnCode.InvalidHandle] = "Invalid handle",
            [Rfal.ReturnCode.IncompleteByte] = "Incomplete byte",
            [Rfal.ReturnCode.IncompleteByte01] = "Incomplete byte (1 bit)",
            [Rfal.ReturnCode.IncompleteByte02] = "Incomplete byte (2 bits)",
            [Rfal.ReturnCode.IncompleteByte03] = "Incomplete byte (3 bits)",
            [Rfal.ReturnCode.IncompleteByte04] = "Incomplete byte (4 bits)",
            [Rfal.ReturnCode.IncompleteByte05] = "Incomplete byte (5 bits)",
            [Rfal.ReturnCode.IncompleteByte06] = "Incomplete byte (6 bits)",
            [Rfal.ReturnCode.IncompleteByte07] = "Incomplete byte (7 bits)",
        };

    private static readonly IReadOnlyDictionary<Rfal.NfcState, string> States =
        new Dictionary<Rfal.NfcState, string>
        {
            [Rfal.NfcState.NotInit] = "Not initialized",
            [Rfal.NfcState.Idle] = "Idle",
            [Rfal.NfcState.StartDiscovery] = "Start discovery",
            [Rfal.NfcState.WakeupMode] = "Wake-up mode",
            [Rfal.NfcState.PollTechDetect] = "Poll tech detect",
            [Rfal.NfcState.PollColAvoidance] = "Poll collision avoidance",
            [Rfal.NfcState.PollSelect] = "Poll select",
            [Rfal.NfcState.PollActivation] = "Poll activation",
            [Rfal.NfcState.ListenTechDetect] = "Listen tech detect",
            [Rfal.NfcState.ListenColAvoidance] = "Listen collision avoidance",
            [Rfal.NfcState.ListenActivation] = "Listen activation",
            [Rfal.NfcState.ListenSleep] = "Listen sleep",
            [Rfal.NfcState.Activated] = "Activated",
            [Rfal.NfcState.DataExchange] = "Data exchange",
            [Rfal.NfcState.DataExchangeDone] = "Data exchange done",
            [Rfal.NfcState.Deactivation] = "Deactivation",
        };

    private static readonly IReadOnlyDictionary<Rfal.NfcDevType, string> DevTypes =
        new Dictionary<Rfal.NfcDevType, string>
        {
            [Rfal.NfcDevType.ListenNfca] = "Listen NFC-A",
            [Rfal.NfcDevType.ListenNfcb] = "Listen NFC-B",
            [Rfal.NfcDevType.ListenNfcf] = "Listen NFC-F",
            [Rfal.NfcDevType.ListenNfcv] = "Listen NFC-V",
            [Rfal.NfcDevType.ListenSt25tb] = "Listen ST25TB",
            [Rfal.NfcDevType.ListenAp2p] = "Listen AP2P",
            [Rfal.NfcDevType.ListenProp] = "Listen Proprietary",
            [Rfal.NfcDevType.PollNfca] = "Poll NFC-A",
            [Rfal.NfcDevType.PollNfcb] = "Poll NFC-B",
            [Rfal.NfcDevType.PollNfcf] = "Poll NFC-F",
            [Rfal.NfcDevType.PollNfcv] = "Poll NFC-V",
            [Rfal.NfcDevType.PollAp2p] = "Poll AP2P",
        };

    public static string DescribeReturnCode(ushort value)
        => DescribeEnum((Rfal.ReturnCode)value, ReturnCodes, "Unknown return code");

    public static string DescribeState(uint value)
        => DescribeEnum((Rfal.NfcState)value, States, "Unknown state");

    public static string DescribeDevType(uint value)
        => DescribeEnum((Rfal.NfcDevType)value, DevTypes, "Unknown dev type");

    private static string DescribeEnum<T>(T value, IReadOnlyDictionary<T, string> map, string fallback)
        where T : struct
    {
        if (map.TryGetValue(value, out var label))
            return label;
        return fallback;
    }
}
