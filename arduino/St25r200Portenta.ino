#include <Arduino.h>
#include <Ethernet.h>
#include <mbed.h>

#include "St25r200Reader.h"
#include "RestNotifier.h"

using rtos::Thread;

// === Network config (update to your LAN) ===
byte kMac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress kLocalIp(192, 168, 1, 50);
IPAddress kGateway(192, 168, 1, 1);
IPAddress kSubnet(255, 255, 255, 0);

RestConfig restConfig = {
    IPAddress(192, 168, 1, 100),
    80,
    "/",
    200,
};

RestNotifier notifier(restConfig);

St25r200Reader::Options readerAOptions = {
    &Serial1,
    115200,
    400,
    400,
    75,
    4,
    St25r200Reader::LogFrames,
};

St25r200Reader::Options readerBOptions = {
    &Serial2,
    115200,
    400,
    400,
    75,
    4,
    St25r200Reader::LogFrames,
};

St25r200Reader readerA(readerAOptions, notifier, Serial);
St25r200Reader readerB(readerBOptions, notifier, Serial);

Thread readerAThread;
Thread readerBThread;

void readerTaskA()
{
    readerA.begin();
    readerA.loop();
}

void readerTaskB()
{
    readerB.begin();
    readerB.loop();
}

void setup()
{
    Serial.begin(115200);
    while (!Serial) { delay(10); }

    Ethernet.begin(kMac, kLocalIp, kGateway, kGateway, kSubnet);

    readerAThread.start(readerTaskA);
    readerBThread.start(readerTaskB);
}

void loop()
{
    delay(1000);
}
