#pragma once

#include <Arduino.h>
#include <Ethernet.h>

struct RestConfig
{
    IPAddress host;
    uint16_t port = 80;
    const char* basePath = "/";
    uint16_t timeoutMs = 200;
};

class RestNotifier
{
public:
    explicit RestNotifier(const RestConfig& config)
        : _config(config)
    {
    }

    void postPlaced(const String& uid, Stream& logStream, uint8_t logLevel);
    void postRemoved(const String& uid, Stream& logStream, uint8_t logLevel);

private:
    void post(const char* endpoint, const String& uid, Stream& logStream, uint8_t logLevel);

    RestConfig _config;
};
