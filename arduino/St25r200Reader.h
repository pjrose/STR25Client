#pragma once

#include <Arduino.h>
#include "PresenceTracker.h"
#include "RfalEnums.h"
#include "RestNotifier.h"

class St25r200Reader
{
public:
    enum LogLevel : uint8_t
    {
        LogNone = 0,
        LogErrors = 1,
        LogFrames = 2,
        LogBytes = 3,
    };

    struct Options
    {
        HardwareSerial* serial;
        uint32_t baudRate = 115200;
        uint16_t readTimeoutMs = 400;
        uint16_t writeTimeoutMs = 400;
        uint16_t loopDelayMs = 75;
        uint8_t maxTrackedTags = 4;
        LogLevel logLevel = LogErrors;
    };

    St25r200Reader(const Options& options, RestNotifier& notifier, Stream& logStream);

    void begin();
    void loop();

private:
    void rfalNfcInitialize();
    void rfalNfcDiscover();
    uint32_t rfalNfcGetState();
    void rfalNfcDeactivate(uint32_t deactType);
    void rfalNfcGetDevicesFound(String* uidList, size_t& uidCount);

    void publishPresence(const String* uids, size_t uidCount);

    void sendAndReceive(SerCommandId requestCmdId, const uint8_t* payload, size_t payloadLen,
                        uint8_t* rspBuf, size_t& rspLen);
    bool readFrame(uint16_t& cmdId, uint8_t* payload, size_t& payloadLen, uint8_t* rawFrame, size_t& rawLen);

    bool readExact(uint8_t* buffer, size_t count);

    static void writeU16BE(uint8_t* b, size_t& ofs, uint16_t v);
    static void writeU32BE(uint8_t* b, size_t& ofs, uint32_t v);
    static uint16_t readU16BE(const uint8_t* buf, size_t ofs);
    static uint32_t readU32BE(const uint8_t* buf, size_t ofs);
    static void bytesToHex(const uint8_t* bytes, size_t len, char* out, size_t outLen);

    static void buildDiscoverParams(uint8_t* outBuf, size_t& outLen);

    HardwareSerial* _serial;
    Options _opt;
    PresenceTracker _tracker;
    RestNotifier& _notifier;
    Stream& _log;

    static constexpr uint8_t FrameHeader = 0xAA;
};
