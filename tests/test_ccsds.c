#include <stdio.h>
#include <string.h>
#include "ccsds.h"

// Simple macro for checking test conditions
#define ASSERT_EQ(expected, actual, msg)                                    \
    if ((expected) != (actual))                                             \
    {                                                                       \
        printf("❌ %s (expected %d, got %d)\n", msg, (expected), (actual)); \
        return 1;                                                           \
    }                                                                       \
    else                                                                    \
    {                                                                       \
        printf("✅ %s\n", msg);                                             \
    }

int main(void)
{
    printf("Running CCSDS tests...\n\n");

    // --- TEST 1: Builder rejects invalid version ---
    ccsds_primary_header_t bad_header;
    ccsds_error_t err = build_primary_header(
        &bad_header,
        9, // === invalid version (should be 0–7) ===
        1, 1, 100, 2, 42, 15);
    ASSERT_EQ(CCSDS_ERR_VERSION, err, "Builder fails for version > 7");

    // --- TEST 2: Builder creates a valid header and survives pack/unpack ---
    ccsds_primary_header_t header;
    ccsds_error_t ok = build_primary_header(
        &header,
        0,     // === valid version ===
        1,     // type
        1,     // sec_hdr_flag
        100,   // apid
        3,     // seq_flags
        16383, // max seq_count
        65535  // max length
    );
    ASSERT_EQ(CCSDS_OK, ok, "Builder accepts valid input");

    // --- TEST 3: Pack + unpack round trip preserves values ---
    uint8_t raw[6];
    pack_ccsds_header(raw, &header);

    ccsds_primary_header_t decoded;
    unpack_ccsds_header(raw, &decoded);

    ASSERT_EQ(header.version, decoded.version, "Version preserved after pack/unpack");
    ASSERT_EQ(header.type, decoded.type, "Type preserved after pack/unpack");
    ASSERT_EQ(header.sec_hdr_flag, decoded.sec_hdr_flag, "Secondary header flag preserved");
    ASSERT_EQ(header.apid, decoded.apid, "APID preserved after pack/unpack");
    ASSERT_EQ(header.seq_flags, decoded.seq_flags, "Sequence flags preserved after pack/unpack");
    ASSERT_EQ(header.seq_count, decoded.seq_count, "Sequence count preserved after pack/unpack");
    ASSERT_EQ(header.length, decoded.length, "Length preserved after pack/unpack");

    printf("\n All tests passed!\n");
    return 0;
}
