#include "RestNotifier.h"

void RestNotifier::postPlaced(const String& uid, Stream& logStream, uint8_t logLevel)
{
    post("placed", uid, logStream, logLevel);
}

void RestNotifier::postRemoved(const String& uid, Stream& logStream, uint8_t logLevel)
{
    post("removed", uid, logStream, logLevel);
}

void RestNotifier::post(const char* endpoint, const String& uid, Stream& logStream, uint8_t logLevel)
{
    EthernetClient client;
    client.setTimeout(_config.timeoutMs);

    unsigned long start = millis();
    if (!client.connect(_config.host, _config.port))
    {
        if (logLevel >= 1)
        {
            logStream.print("HTTP connect failed to ");
            logStream.print(_config.host);
            logStream.print(":");
            logStream.println(_config.port);
        }
        client.stop();
        return;
    }

    String path = String(_config.basePath) + endpoint;
    String payload = String("{\"uid\":\"") + uid + "\"}";

    client.print("POST ");
    client.print(path);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(_config.host);
    client.println("Connection: close");
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(payload.length());
    client.println();
    client.print(payload);

    while (client.connected() && (millis() - start) < _config.timeoutMs)
    {
        if (client.available())
        {
            client.read();
        }
        delay(1);
    }

    if (logLevel >= 2)
    {
        logStream.print("HTTP POST ");
        logStream.print(path);
        logStream.print(" uid=");
        logStream.print(uid);
        logStream.println(" done");
    }

    client.stop();
}
