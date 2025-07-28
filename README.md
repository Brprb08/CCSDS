# CCSDS Packet Library

This is a small C library for encoding and decoding CCSDS (Consultative Committee for Space Data Systems) space packet headers.  
I started this as a way to learn low‑level bit packing in C, but it’s turning into a useful base for working with CCSDS‑formatted telemetry and telecommand data.

## What it does right now

- Builds and validates a CCSDS **primary header** (6 bytes)
- Optionally encodes/decodes a **secondary header** (currently just coarse/fine time — 8 bytes)
- Encodes headers into a byte buffer and unpacks them back into structs
- Simple example program (`main.c`) showing how to build, encode, and decode a packet

## Why CCSDS?

CCSDS is a space industry standard for packetizing telemetry/command data for spacecraft.  
Most satellites and ground systems use some variation of it, so it’s a good format to learn.

## Next steps / ideas

- Add payload support with length checking
- Add support for more secondary header types (e.g. command headers)
- Write unit tests and fuzz tests for decoding
- Maybe turn this into a tiny CLI tool for inspecting CCSDS packets

## Building

Right now you can just build it with `gcc`:

## Tests

**Tests are not working at the moment, will be fixed in next commit.**  
_This message will be gone when that happens._

## How to run

```bash
make clean
make
make run
./ccsds
```
