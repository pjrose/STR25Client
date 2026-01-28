# ST25R200 Open-Source Arduino (Portenta H7)

This folder contains a **FreeRTOS/mbed**-based Arduino implementation for the ST25R200 serial RFAL protocol.
It focuses on **ISO15693 (NFC-V) presence** and posts REST events over Ethernet.

## Highlights
- Two concurrent reader instances (UARTs on the 40-pin HAT carrier header).
- Tracks up to **4 tags** per reader.
- Logs raw hex frames and decodes enum values into plain English (configurable log level).
- REST POST to `/placed` and `/removed` with a short timeout.

## Files
- `St25r200Portenta.ino`: main sketch (two RTOS threads).
- `St25r200Reader.h/.cpp`: protocol + NFC-V presence loop.
- `RfalEnums.h`: enum mirror and human-readable decoding.
- `PresenceTracker.h`: max-4-tag delta tracking.
- `RestNotifier.h/.cpp`: Ethernet HTTP POST helper.

## Setup
1. Update the network config and REST host in `St25r200Portenta.ino`.
2. Wire the ST25R200 UARTs to the Portenta H7 40-pin header (Serial1/Serial2).
3. Build & flash.

## Logging
`LogLevel` is configurable in `St25r200Reader::Options`:
- `LogErrors` (default) only logs errors.
- `LogFrames` logs TX/RX frames and decoded state.
- `LogBytes` logs resync bytes (very verbose).
