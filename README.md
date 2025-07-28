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

## Tests

**Tests are not working at the moment, will be fixed in next commit.**  
_This message will be gone when that happens._

## Building

You’ll need a C compiler (GCC or Clang).
On Linux/macOS this is usually pre‑installed or easy to get.
On Windows, you can install GCC via MSYS2 or MinGW (and then the manual gcc command below will work fine).

### Using the Makefile (Linux/macOS/WSL only):

```bash
make clean     # remove old builds
make           # compile the project
make run       # run the compiled program
```

### Compiling manually with gcc

```bash
gcc -Wall -Wextra -std=c11 -Iinclude src/ccsds.c main.c -o ccsds_demo
./ccsds_demo
```
