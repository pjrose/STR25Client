using System;

namespace St25R200.OpenSource;

/// <summary>
/// Open-source C# representations of RFAL enums/constants from the ST25R200 Eval GUI headers.
/// Reference headers: rfal_nfc.h, rfal_rf.h, rfal_utils.h, rfal_nfcv.h.
/// </summary>
public static class Rfal
{
    // rfal_utils.h
    public enum ReturnCode : ushort
    {
        None = 0,
        NoMem = 1,
        Busy = 2,
        Io = 3,
        Timeout = 4,
        Request = 5,
        NoMsg = 6,
        Param = 7,
        System = 8,
        Framing = 9,
        Overrun = 10,
        Proto = 11,
        Internal = 12,
        Again = 13,
        MemCorrupt = 14,
        NotImplemented = 15,
        PcCorrupt = 16,
        Send = 17,
        Ignore = 18,
        Semantic = 19,
        Syntax = 20,
        Crc = 21,
        NotFound = 22,
        NotUnique = 23,
        NotSupported = 24,
        Write = 25,
        Fifo = 26,
        Parity = 27,
        Done = 28,
        RfCollision = 29,
        HwOverrun = 30,
        ReleaseReq = 31,
        SleepReq = 32,
        WrongState = 33,
        MaxReruns = 34,
        Disabled = 35,
        HwMismatch = 36,
        LinkLoss = 37,
        InvalidHandle = 38,
        IncompleteByte = 40,
        IncompleteByte01 = 41,
        IncompleteByte02 = 42,
        IncompleteByte03 = 43,
        IncompleteByte04 = 44,
        IncompleteByte05 = 45,
        IncompleteByte06 = 46,
        IncompleteByte07 = 47,
    }

    // rfal_nfc.h
    public enum NfcState : uint
    {
        NotInit = 0,
        Idle = 1,
        StartDiscovery = 2,
        WakeupMode = 3,
        PollTechDetect = 10,
        PollColAvoidance = 11,
        PollSelect = 12,
        PollActivation = 13,
        ListenTechDetect = 20,
        ListenColAvoidance = 21,
        ListenActivation = 22,
        ListenSleep = 23,
        Activated = 30,
        DataExchange = 31,
        DataExchangeDone = 33,
        Deactivation = 34,
    }

    public enum NfcDevType : uint
    {
        ListenNfca = 0,
        ListenNfcb = 1,
        ListenNfcf = 2,
        ListenNfcv = 3,
        ListenSt25tb = 4,
        ListenAp2p = 5,
        ListenProp = 6,
        PollNfca = 10,
        PollNfcb = 11,
        PollNfcf = 12,
        PollNfcv = 13,
        PollAp2p = 15,
    }

    public enum NfcDeactivateType : uint
    {
        Idle = 0,
        Sleep = 1,
        Discovery = 2,
    }

    [Flags]
    public enum NfcPollTech : ushort
    {
        None = 0x0000,
        A = 0x0001,
        B = 0x0002,
        F = 0x0004,
        V = 0x0008,
        Ap2p = 0x0010,
        St25tb = 0x0020,
        Prop = 0x0040,
        ListenA = 0x1000,
        ListenB = 0x2000,
        ListenF = 0x4000,
        ListenAp2p = 0x8000,
    }

    // rfal_rf.h
    public enum BitRate : uint
    {
        Br106 = 0,
        Br212 = 1,
        Br424 = 2,
        Br848 = 3,
        Br1695 = 4,
        Br3390 = 5,
        Br6780 = 6,
        Br13560 = 7,
        Br211p88 = 0xE9,
        Br105p94 = 0xEA,
        Br52p97 = 0xEB,
        Br26p48 = 0xEC,
        Br1p66 = 0xED,
        Keep = 0xFF,
    }

    public enum ComplianceMode : uint
    {
        Nfc = 0,
        Emv = 1,
        Iso = 2,
    }

    // rfal_nfcv.h
    public const int NfcvUidLength = 8;

    public enum NfcvRequestFlags : byte
    {
        Default = 0x02,
        SubCarrier = 0x01,
        DataRate = 0x02,
        Inventory = 0x04,
        ProtocolExtension = 0x08,
        Select = 0x10,
        Address = 0x20,
        Option = 0x40,
        Rfu = 0x80,
        Afi = 0x10,
        NbSlots = 0x20,
    }
}

public enum SerCommandId : ushort
{
    // From the serial protocol used by serHost/serRfal; request IDs are even, response is request+1.
    RfalNfcInitializeReq = 0x2000,
    RfalNfcDiscoverReq = 0x2002,
    RfalNfcGetStateReq = 0x2004,
    RfalNfcGetDevicesReq = 0x2006,
    RfalNfcDeactivateReq = 0x2010,
    
    // System error notification (unsolicited from device)
    SysErrorReq = 0xF00C,
    SysErrorRsp = 0xF00D,
}
