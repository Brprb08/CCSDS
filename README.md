# CCSDS Packet Library

A lightweight C library for **encoding and decoding CCSDS (Consultative Committee for Space Data Systems) packets**, the industry standard format used by NASA, ESA, and many satellites for telemetry and telecommand.

This started as a way to practice bit‑level packing in C, but it’s evolving into a base for working with CCSDS‑formatted data (and could grow into a packet inspection tool).

---

## Features

- Build & validate a CCSDS **Primary Header** (6 bytes)  
- Encode/decode a **Secondary Header** (currently supports CUC_TIME [coarse/fine time] – 8 bytes)  
- Serialize headers into raw byte buffers & unpack them back into structs  
- Includes **unit tests** for packing/unpacking correctness  
- Simple CLI demo (`main.c`) to build, encode, and decode packets

---

## Why CCSDS?

The **CCSDS Space Packet Protocol** is the foundation for spacecraft data exchange.  
Most satellites and ground systems use it (or a derivative), so understanding it is essential for aerospace software work.

---

## Example Output

Running the CLI demo:
1. CCSDS Unsegmented Time Code
```bash
./ccsds_demo CUC_TIME 12345 54321
```

Produces:
```bash
[Encoded CCSDS Packet] (14 bytes)
18 64 80 2A 00 0F 68 8E C9 C4 7F FF FF FF

[Decoded Primary Header]
  Version: 0
  Type: 1 (0=Telemetry, 1=Telecommand)
  Secondary Header Flag: 1
  APID: 100
  Sequence Flags: 2
  Sequence Count: 42
  Packet Length: 15 (payload bytes: 16)

[Decoded Secondary Header] (unpacked 6 bytes)
  Coarse Time: 1754188228
  Fine Time: 2147483647
```
---

2. TeleCommand – Packet Utilization Standard
```bash
make run ARGS="TC_PUS 17 165 0"
```

Produces:
```bash
[Encoded CCSDS Packet] (9 bytes)
18 64 80 2A 00 0F 11 A5  00

[Decoded Primary Header]
  Version: 0
  Type: 1 (0=Telemetry, 1=Telecommand)
  Secondary Header Flag: 1
  APID: 100
  Sequence Flags: 2
  Sequence Count: 42
  Packet Length: 15 (payload bytes: 16)

[Decoded Secondary Header] (unpacked 3 bytes)
  Function Code: 17
  Checksum: 165
  Spare: 0
```

## Building
You’ll need a C compiler (GCC or Clang).

### Using the Makefile (Linux/macOS/WSL):
```bash
make clean       # remove old builds
make             # compile the project
make run         # run the compiled demo program
make test        # run the test suite
```

### Manual Compile:
```bash
gcc -Wall -Wextra -std=c11 -Iinclude src/*.c main.c -o ccsds_demo
./ccsds_demo
```

## Tests
Run the tests with:
```bash
make test
```
You’ll see ✅ for each check and:
```bash
All tests passed!
```

## Next Steps
- Add payload support with proper length checking
- Support additional secondary header types (e.g. TC_PUS commands)
- Expand the CLI into a CCSDS packet inspector
- Add fuzz tests for robustness





