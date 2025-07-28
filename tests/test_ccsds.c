#include <stdio.h>
#include <string.h>
#include "ccsds.h"

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

// TESTS

int test_invalid_primary_header()
{
    ccsds_primary_header_t bad_header;
    ccsds_error_t err = build_primary_header(&bad_header,
                                             9, // invalid version (should be 0–7)
                                             1, 1, 100, 2, 42, 15);
    ASSERT_EQ(CCSDS_ERR_VERSION, err, "Builder fails for version > 7");
    return 0;
}

int test_valid_primary_header()
{
    ccsds_primary_header_t header;
    ccsds_error_t ok = build_primary_header(&header,
                                            0,     // valid version
                                            1,     // type
                                            1,     // sec_hdr_flag
                                            100,   // apid
                                            3,     // seq_flags
                                            16383, // max seq_count
                                            65535  // max length
    );
    ASSERT_EQ(CCSDS_OK, ok, "Builder accepts valid input");
    return 0;
}

int test_primary_pack_unpack()
{
    ccsds_primary_header_t header;
    build_primary_header(&header, 0, 1, 1, 100, 3, 16383, 65535);

    uint8_t raw[6];
    pack_ccsds_primary_header(raw, &header);

    ccsds_primary_header_t decoded;
    unpack_ccsds_primary_header(raw, &decoded);

    ASSERT_EQ(header.version, decoded.version, "Version preserved after pack/unpack");
    ASSERT_EQ(header.type, decoded.type, "Type preserved after pack/unpack");
    ASSERT_EQ(header.sec_hdr_flag, decoded.sec_hdr_flag, "Sec header flag preserved");
    ASSERT_EQ(header.apid, decoded.apid, "APID preserved after pack/unpack");
    ASSERT_EQ(header.seq_flags, decoded.seq_flags, "Seq flags preserved after pack/unpack");
    ASSERT_EQ(header.seq_count, decoded.seq_count, "Seq count preserved after pack/unpack");
    ASSERT_EQ(header.length, decoded.length, "Length preserved after pack/unpack");
    return 0;
}

int test_secondary_header()
{
    ccsds_secondary_header_t sec_header;
    uint32_t coarse = 1753658202; // hex(68 86 B3 5A)
    uint32_t fine = 1288490188;   // hex(4C CC CC CC)
    ccsds_error_t sec_ok = build_secondary_header(&sec_header, coarse, fine);
    ASSERT_EQ(CCSDS_OK, sec_ok, "Secondary header builder accepts valid input");

    uint8_t fake_buf[14] = {0}; // Simulate full packet
    pack_ccsds_secondary_header(fake_buf, &sec_header);

    ccsds_secondary_header_t sec_decoded;
    unpack_ccsds_secondary_header(fake_buf, &sec_decoded);

    ASSERT_EQ(sec_header.coarse_time, sec_decoded.coarse_time, "Coarse time preserved");
    ASSERT_EQ(sec_header.fine_time, sec_decoded.fine_time, "Fine time preserved");
    return 0;
}

int main(void)
{
    printf("Running CCSDS tests...\n\n");

    if (test_invalid_primary_header())
        return 1;
    if (test_valid_primary_header())
        return 1;
    if (test_primary_pack_unpack())
        return 1;
    if (test_secondary_header())
        return 1;

    printf("\n All tests passed!\n");
    return 0;
}
