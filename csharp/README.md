# ST25R200 Open-Source C# Service (NFC-V / ISO15693)

This folder contains a dependency-free C# implementation of the ST25R200 serial RFAL protocol
with **NFC-V tag presence only**. It is based on the public headers in `ST25R200_Eval_GUI/Exe_Demos/Includes`
for enum names, constants, and serialized struct layout.

## Key behavior
- Detects ISO15693 (NFC-V) tag presence only.
- Tracks **up to 4 tags** at a time (configurable).
- When a tag appears: POST to `http://<local-ip>/placed`.
- When a tag leaves: POST to `http://<local-ip>/removed`.
- REST timeout defaults to **200 ms** (fast failure when server is not listening).

## Files
- `St25r200ReaderService.cs`: serial protocol + NFC-V presence loop.
- `RfalEnums.cs`: enums/constants mirrored from the ST headers.
- `RfalDecoders.cs`: plain-English decode of enums for logging.
- `PresenceTracker.cs`: presence delta tracking with max tag cap.
- `RestNotifier.cs`: REST POST helper with short timeout.
- `Program.cs`: sample console host.

## Notes
- Update `RestBaseUri` in `Program.cs` or `Options` to your local IP.
- The device list parsing follows `serFlatRfalNfcDevice` layout from `serCommonTypes.h`.
