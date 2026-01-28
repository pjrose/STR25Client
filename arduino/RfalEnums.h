#pragma once

// Open-source RFAL enum mirror from ST25R200_Eval_GUI headers.

#include <Arduino.h>

namespace Rfal
{
    enum ReturnCode : uint16_t
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
    };

    enum NfcState : uint32_t
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
    };

    enum NfcDevType : uint32_t
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
    };

    enum NfcDeactivateType : uint32_t
    {
        Idle = 0,
        Sleep = 1,
        Discovery = 2,
    };

    enum BitRate : uint32_t
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
    };

    enum ComplianceMode : uint32_t
    {
        Nfc = 0,
        Emv = 1,
        Iso = 2,
    };

    enum NfcPollTech : uint16_t
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
    };

    constexpr uint8_t NfcvUidLength = 8;

    inline const char* DescribeReturnCode(uint16_t value)
    {
        switch (value)
        {
            case None: return "No error";
            case NoMem: return "Not enough memory";
            case Busy: return "Device/resource busy";
            case Io: return "Generic I/O error";
            case Timeout: return "Timeout";
            case Request: return "Invalid request";
            case NoMsg: return "No message";
            case Param: return "Parameter error";
            case System: return "System error";
            case Framing: return "Framing error";
            case Overrun: return "Overrun error";
            case Proto: return "Protocol error";
            case Internal: return "Internal error";
            case Again: return "Try again";
            case MemCorrupt: return "Memory corruption";
            case NotImplemented: return "Not implemented";
            case PcCorrupt: return "Program counter corruption";
            case Send: return "Send error";
            case Ignore: return "Ignored error";
            case Semantic: return "Semantic error";
            case Syntax: return "Syntax error";
            case Crc: return "CRC error";
            case NotFound: return "Transponder not found";
            case NotUnique: return "Transponder not unique";
            case NotSupported: return "Operation not supported";
            case Write: return "Write error";
            case Fifo: return "FIFO error";
            case Parity: return "Parity error";
            case Done: return "Transfer already finished";
            case RfCollision: return "RF collision";
            case HwOverrun: return "HW overrun";
            case ReleaseReq: return "Release requested";
            case SleepReq: return "Sleep requested";
            case WrongState: return "Wrong state";
            case MaxReruns: return "Max reruns";
            case Disabled: return "Disabled";
            case HwMismatch: return "HW mismatch";
            case LinkLoss: return "Link loss";
            case InvalidHandle: return "Invalid handle";
            case IncompleteByte: return "Incomplete byte";
            case IncompleteByte01: return "Incomplete byte (1 bit)";
            case IncompleteByte02: return "Incomplete byte (2 bits)";
            case IncompleteByte03: return "Incomplete byte (3 bits)";
            case IncompleteByte04: return "Incomplete byte (4 bits)";
            case IncompleteByte05: return "Incomplete byte (5 bits)";
            case IncompleteByte06: return "Incomplete byte (6 bits)";
            case IncompleteByte07: return "Incomplete byte (7 bits)";
            default: return "Unknown return code";
        }
    }

    inline const char* DescribeState(uint32_t value)
    {
        switch (value)
        {
            case NotInit: return "Not initialized";
            case Idle: return "Idle";
            case StartDiscovery: return "Start discovery";
            case WakeupMode: return "Wake-up mode";
            case PollTechDetect: return "Poll tech detect";
            case PollColAvoidance: return "Poll collision avoidance";
            case PollSelect: return "Poll select";
            case PollActivation: return "Poll activation";
            case ListenTechDetect: return "Listen tech detect";
            case ListenColAvoidance: return "Listen collision avoidance";
            case ListenActivation: return "Listen activation";
            case ListenSleep: return "Listen sleep";
            case Activated: return "Activated";
            case DataExchange: return "Data exchange";
            case DataExchangeDone: return "Data exchange done";
            case Deactivation: return "Deactivation";
            default: return "Unknown state";
        }
    }

    inline const char* DescribeDevType(uint32_t value)
    {
        switch (value)
        {
            case ListenNfca: return "Listen NFC-A";
            case ListenNfcb: return "Listen NFC-B";
            case ListenNfcf: return "Listen NFC-F";
            case ListenNfcv: return "Listen NFC-V";
            case ListenSt25tb: return "Listen ST25TB";
            case ListenAp2p: return "Listen AP2P";
            case ListenProp: return "Listen Proprietary";
            case PollNfca: return "Poll NFC-A";
            case PollNfcb: return "Poll NFC-B";
            case PollNfcf: return "Poll NFC-F";
            case PollNfcv: return "Poll NFC-V";
            case PollAp2p: return "Poll AP2P";
            default: return "Unknown dev type";
        }
    }
}

enum class SerCommandId : uint16_t
{
    RfalNfcInitializeReq = 0x2000,
    RfalNfcDiscoverReq = 0x2002,
    RfalNfcGetStateReq = 0x2004,
    RfalNfcGetDevicesReq = 0x2006,
    RfalNfcDeactivateReq = 0x2010,
};
