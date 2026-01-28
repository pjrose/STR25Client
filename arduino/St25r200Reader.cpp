#include "St25r200Reader.h"

St25r200Reader::St25r200Reader(const Options& options, RestNotifier& notifier, Stream& logStream)
    : _serial(options.serial)
    , _opt(options)
    , _tracker(options.maxTrackedTags)
    , _notifier(notifier)
    , _log(logStream)
{
}

void St25r200Reader::begin()
{
    if (_serial)
    {
        _serial->begin(_opt.baudRate);
        _serial->setTimeout(_opt.readTimeoutMs);
    }
}

void St25r200Reader::loop()
{
    if (!_serial)
    {
        return;
    }

    rfalNfcInitialize();
    rfalNfcDiscover();

    while (true)
    {
        uint32_t state = rfalNfcGetState();
        if (_opt.logLevel >= LogFrames)
        {
            _log.print("State=0x");
            _log.print(state, HEX);
            _log.print(" ");
            _log.println(Rfal::DescribeState(state));
        }

        if (state == Rfal::NfcState::Activated)
        {
            String uids[4];
            size_t uidCount = 0;
            rfalNfcGetDevicesFound(uids, uidCount);
            publishPresence(uids, uidCount);

            rfalNfcDeactivate(static_cast<uint32_t>(Rfal::NfcDeactivateType::Idle));
            rfalNfcDiscover();
        }

        delay(_opt.loopDelayMs);
    }
}

void St25r200Reader::rfalNfcInitialize()
{
    uint8_t rsp[8] = {0};
    size_t rspLen = sizeof(rsp);
    sendAndReceive(SerCommandId::RfalNfcInitializeReq, nullptr, 0, rsp, rspLen);
    uint16_t ret = readU16BE(rsp, 0);
    if (ret != Rfal::None)
    {
        _log.print("rfalNfcInitialize failed: ");
        _log.print(ret, HEX);
        _log.print(" ");
        _log.println(Rfal::DescribeReturnCode(ret));
    }
}

void St25r200Reader::rfalNfcDiscover()
{
    uint8_t params[93];
    size_t paramsLen = sizeof(params);
    buildDiscoverParams(params, paramsLen);

    uint8_t rsp[8] = {0};
    size_t rspLen = sizeof(rsp);
    sendAndReceive(SerCommandId::RfalNfcDiscoverReq, params, paramsLen, rsp, rspLen);
    uint16_t ret = readU16BE(rsp, 0);
    if (ret != Rfal::None)
    {
        _log.print("rfalNfcDiscover failed: ");
        _log.print(ret, HEX);
        _log.print(" ");
        _log.println(Rfal::DescribeReturnCode(ret));
    }
}

uint32_t St25r200Reader::rfalNfcGetState()
{
    uint8_t rsp[8] = {0};
    size_t rspLen = sizeof(rsp);
    sendAndReceive(SerCommandId::RfalNfcGetStateReq, nullptr, 0, rsp, rspLen);
    return readU32BE(rsp, 0);
}

void St25r200Reader::rfalNfcDeactivate(uint32_t deactType)
{
    uint8_t payload[4] = {0};
    size_t ofs = 0;
    writeU32BE(payload, ofs, deactType);

    uint8_t rsp[8] = {0};
    size_t rspLen = sizeof(rsp);
    sendAndReceive(SerCommandId::RfalNfcDeactivateReq, payload, sizeof(payload), rsp, rspLen);
    uint16_t ret = readU16BE(rsp, 0);
    if (ret != Rfal::None)
    {
        _log.print("rfalNfcDeactivate failed: ");
        _log.print(ret, HEX);
        _log.print(" ");
        _log.println(Rfal::DescribeReturnCode(ret));
    }
}

void St25r200Reader::rfalNfcGetDevicesFound(String* uidList, size_t& uidCount)
{
    uint8_t rsp[256] = {0};
    size_t rspLen = sizeof(rsp);
    sendAndReceive(SerCommandId::RfalNfcGetDevicesReq, nullptr, 0, rsp, rspLen);

    size_t ofs = 0;
    uint16_t ret = readU16BE(rsp, ofs); ofs += 2;
    if (ret != Rfal::None)
    {
        _log.print("rfalNfcGetDevicesFound failed: ");
        _log.print(ret, HEX);
        _log.print(" ");
        _log.println(Rfal::DescribeReturnCode(ret));
        return;
    }

    uint8_t devCnt = rsp[ofs++];
    uidCount = 0;

    for (uint8_t i = 0; i < devCnt && uidCount < _opt.maxTrackedTags; ++i)
    {
        uint32_t devType = readU32BE(rsp, ofs); ofs += 4;

        if (_opt.logLevel >= LogFrames)
        {
            _log.print("Device[");
            _log.print(i);
            _log.print("] devType=0x");
            _log.print(devType, HEX);
            _log.print(" ");
            _log.println(Rfal::DescribeDevType(devType));
        }

        // NFC-A
        ofs += 4; // nfcaType
        ofs += 2; // sensRes
        ofs += 1; // selRes
        uint8_t nfcId1Len = rsp[ofs++];
        if (nfcId1Len > 32) nfcId1Len = 32;
        ofs += nfcId1Len; // nfcId1
        ofs += 1; // isSleep

        // NFC-B
        ofs += 1; // sensbResLen
        ofs += 5; // sensbRes
        ofs += 1; // isSleep

        // NFC-F
        ofs += 1; // sensfResLen
        ofs += 19; // sensfRes

        // NFC-V
        ofs += 1; // resFlag
        ofs += 1; // dsfid
        const uint8_t* uid = rsp + ofs; ofs += Rfal::NfcvUidLength;
        ofs += 2; // crc
        ofs += 1; // isSleep

        // ST25TB
        ofs += 1; // chipID
        ofs += 8; // UID
        ofs += 1; // isDeselected

        if (devType == static_cast<uint32_t>(Rfal::NfcDevType::ListenNfcv))
        {
            char hexBuf[17] = {0};
            bytesToHex(uid, Rfal::NfcvUidLength, hexBuf, sizeof(hexBuf));
            uidList[uidCount++] = String(hexBuf);
        }
    }
}

void St25r200Reader::publishPresence(const String* uids, size_t uidCount)
{
    PresenceDelta delta = _tracker.update(uids, uidCount);

    for (size_t i = 0; i < delta.arrivedCount; ++i)
    {
        if (_opt.logLevel >= LogFrames)
        {
            _log.print("ARRIVED ");
            _log.println(delta.arrived[i]);
        }
        _notifier.postPlaced(delta.arrived[i], _log, _opt.logLevel);
    }

    for (size_t i = 0; i < delta.leftCount; ++i)
    {
        if (_opt.logLevel >= LogFrames)
        {
            _log.print("LEFT ");
            _log.println(delta.left[i]);
        }
        _notifier.postRemoved(delta.left[i], _log, _opt.logLevel);
    }
}

void St25r200Reader::sendAndReceive(SerCommandId requestCmdId, const uint8_t* payload, size_t payloadLen,
                                    uint8_t* rspBuf, size_t& rspLen)
{
    uint16_t len = static_cast<uint16_t>(2 + payloadLen);
    uint8_t frame[1 + 2 + 2 + 256] = {0};
    frame[0] = FrameHeader;
    size_t ofs = 1;
    writeU16BE(frame, ofs, len);
    writeU16BE(frame, ofs, static_cast<uint16_t>(requestCmdId));
    if (payloadLen > 0)
    {
        memcpy(frame + ofs, payload, payloadLen);
        ofs += payloadLen;
    }

    if (_opt.logLevel >= LogFrames)
    {
        char hexBuf[520] = {0};
        bytesToHex(frame, ofs, hexBuf, sizeof(hexBuf));
        _log.print("TX cmd=0x");
        _log.print(static_cast<uint16_t>(requestCmdId), HEX);
        _log.print(" len=");
        _log.print(len);
        _log.print(" :: ");
        _log.println(hexBuf);
    }

    _serial->write(frame, ofs);

    uint16_t rspCmd = 0;
    uint8_t rawFrame[260] = {0};
    size_t rawLen = sizeof(rawFrame);
    if (!readFrame(rspCmd, rspBuf, rspLen, rawFrame, rawLen))
    {
        _log.println("Read frame failed");
        return;
    }

    uint16_t expectedCmd = static_cast<uint16_t>(requestCmdId) + 1;
    if (rspCmd != expectedCmd && _opt.logLevel >= LogErrors)
    {
        _log.print("Unexpected rsp cmd: 0x");
        _log.print(rspCmd, HEX);
        _log.print(" expected 0x");
        _log.println(expectedCmd, HEX);
    }

    if (_opt.logLevel >= LogFrames)
    {
        char hexBuf[520] = {0};
        bytesToHex(rawFrame, rawLen, hexBuf, sizeof(hexBuf));
        _log.print("RX cmd=0x");
        _log.print(rspCmd, HEX);
        _log.print(" payloadLen=");
        _log.print(rspLen);
        _log.print(" raw=");
        _log.println(hexBuf);
    }
}

bool St25r200Reader::readFrame(uint16_t& cmdId, uint8_t* payload, size_t& payloadLen, uint8_t* rawFrame, size_t& rawLen)
{
    uint16_t skipped = 0;
    while (true)
    {
        int b = _serial->read();
        if (b < 0)
        {
            return false;
        }
        if (static_cast<uint8_t>(b) == FrameHeader)
        {
            break;
        }
        if (_opt.logLevel >= LogBytes && skipped < 64)
        {
            _log.print("Resync skip byte: 0x");
            _log.println(static_cast<uint8_t>(b), HEX);
        }
        skipped++;
        if (skipped > 4096)
        {
            return false;
        }
    }

    uint8_t lenBuf[2] = {0};
    if (!readExact(lenBuf, sizeof(lenBuf)))
        return false;

    uint16_t len = (static_cast<uint16_t>(lenBuf[0]) << 8) | lenBuf[1];
    if (len < 2)
        return false;

    uint8_t cmdBuf[2] = {0};
    if (!readExact(cmdBuf, sizeof(cmdBuf)))
        return false;

    cmdId = (static_cast<uint16_t>(cmdBuf[0]) << 8) | cmdBuf[1];

    size_t pl = len - 2;
    if (pl > payloadLen)
        pl = payloadLen;

    if (!readExact(payload, pl))
        return false;

    payloadLen = pl;

    size_t total = 1 + 2 + 2 + pl;
    if (rawLen >= total)
    {
        rawFrame[0] = FrameHeader;
        memcpy(rawFrame + 1, lenBuf, 2);
        memcpy(rawFrame + 3, cmdBuf, 2);
        memcpy(rawFrame + 5, payload, pl);
        rawLen = total;
    }

    return true;
}

bool St25r200Reader::readExact(uint8_t* buffer, size_t count)
{
    size_t readTotal = 0;
    unsigned long start = millis();
    while (readTotal < count)
    {
        int n = _serial->readBytes(reinterpret_cast<char*>(buffer + readTotal), count - readTotal);
        if (n <= 0)
        {
            if ((millis() - start) > _opt.readTimeoutMs)
                return false;
            continue;
        }
        readTotal += n;
    }
    return true;
}

void St25r200Reader::writeU16BE(uint8_t* b, size_t& ofs, uint16_t v)
{
    b[ofs++] = static_cast<uint8_t>((v >> 8) & 0xFF);
    b[ofs++] = static_cast<uint8_t>(v & 0xFF);
}

void St25r200Reader::writeU32BE(uint8_t* b, size_t& ofs, uint32_t v)
{
    b[ofs++] = static_cast<uint8_t>((v >> 24) & 0xFF);
    b[ofs++] = static_cast<uint8_t>((v >> 16) & 0xFF);
    b[ofs++] = static_cast<uint8_t>((v >> 8) & 0xFF);
    b[ofs++] = static_cast<uint8_t>(v & 0xFF);
}

uint16_t St25r200Reader::readU16BE(const uint8_t* buf, size_t ofs)
{
    return (static_cast<uint16_t>(buf[ofs]) << 8) | buf[ofs + 1];
}

uint32_t St25r200Reader::readU32BE(const uint8_t* buf, size_t ofs)
{
    return (static_cast<uint32_t>(buf[ofs]) << 24) |
           (static_cast<uint32_t>(buf[ofs + 1]) << 16) |
           (static_cast<uint32_t>(buf[ofs + 2]) << 8) |
           (static_cast<uint32_t>(buf[ofs + 3]));
}

void St25r200Reader::bytesToHex(const uint8_t* bytes, size_t len, char* out, size_t outLen)
{
    static const char hex[] = "0123456789ABCDEF";
    if (outLen < (len * 2 + 1))
        return;
    for (size_t i = 0; i < len; ++i)
    {
        out[i * 2] = hex[(bytes[i] >> 4) & 0x0F];
        out[i * 2 + 1] = hex[bytes[i] & 0x0F];
    }
    out[len * 2] = '\0';
}

void St25r200Reader::buildDiscoverParams(uint8_t* outBuf, size_t& outLen)
{
    if (outLen < 93)
        return;

    size_t o = 0;
    writeU32BE(outBuf, o, static_cast<uint32_t>(Rfal::ComplianceMode::Nfc)); // compMode
    writeU16BE(outBuf, o, static_cast<uint16_t>(Rfal::NfcPollTech::V)); // techs2Find
    writeU16BE(outBuf, o, static_cast<uint16_t>(Rfal::NfcPollTech::None)); // techs2Bail
    writeU16BE(outBuf, o, 0x00C8); // totalDuration (200ms)
    outBuf[o++] = 0x04; // devLimit
    writeU32BE(outBuf, o, static_cast<uint32_t>(Rfal::BitRate::Keep)); // maxBR
    writeU32BE(outBuf, o, static_cast<uint32_t>(Rfal::BitRate::Br212)); // nfcfBR
    o += 10; // nfcid3
    o += 48; // GB
    outBuf[o++] = 0x00; // GBLen
    writeU32BE(outBuf, o, static_cast<uint32_t>(Rfal::BitRate::Br424)); // ap2pBR
    outBuf[o++] = 0x00; // p2pNfcaPrio
    writeU32BE(outBuf, o, 0x00000008); // isoDepFS
    outBuf[o++] = 0x03; // nfcDepLR

    o += 17; // lmConfigPA
    o += 21; // lmConfigPF

    outBuf[o++] = 0x00; // wakeupEnabled
    outBuf[o++] = 0x00; // wakeupConfigDefault
    o += 18; // wakeupConfig
    outBuf[o++] = 0x00; // wakeupPollBefore
    writeU16BE(outBuf, o, 0x0000); // wakeupNPolls

    outLen = 93;
}
