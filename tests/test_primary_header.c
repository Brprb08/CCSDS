#include "test_helpers.h"

int test_invalid_primary_header(void)
{
    ccsds_primary_header_t h;
    ASSERT_EQ(CCSDS_ERR_VERSION, build_primary_header(&h, 9, 1, 1, 100, 2, 42, 15),
              "Builder fails for version > 7");
    ASSERT_EQ(CCSDS_ERR_TYPE, build_primary_header(&h, 0, 2, 1, 100, 2, 42, 15),
              "Builder fails for type > 1");
    ASSERT_EQ(CCSDS_ERR_APID, build_primary_header(&h, 0, 1, 1, 3000, 2, 42, 15),
              "Builder fails for APID > 2047");
    ASSERT_EQ(CCSDS_ERR_SEQ_FLAGS, build_primary_header(&h, 0, 1, 1, 100, 4, 42, 15),
              "Builder fails for seq_flags > 3");
    ASSERT_EQ(CCSDS_ERR_SEQ_COUNT, build_primary_header(&h, 0, 1, 1, 100, 2, 20000, 15),
              "Builder fails for seq_count > 16383");
    ASSERT_EQ(CCSDS_ERR_LENGTH, build_primary_header(&h, 0, 1, 1, 100, 2, 42, 70000),
              "Builder fails for length > 65535");
    return 0;
}

int test_valid_primary_header_and_bytes(void)
{
    ccsds_primary_header_t h = make_primary_ok();
    uint8_t buf[64] = {0};
    size_t written = 0;

    ASSERT_EQ(CCSDS_OK, pack_ccsds_primary_header(buf, CCSDS_MAX_PACKET_SIZE, &h, &written),
              "Primary pack returns OK");
    ASSERT_EQ(CCSDS_PRIMARY_HEADER_SIZE, written, "Primary pack out_len is 6");

    uint8_t expected[6] = { 0x18, 0x64, 0xFF, 0xFF, 0xFF, 0xFF };
    ASSERT_MEMEQ(expected, buf, 6, "Primary packed bytes match");

    ccsds_primary_header_t d = {0};
    unpack_ccsds_primary_header(buf, &d);
    ASSERT_EQ(h.version, d.version, "Version preserved");
    ASSERT_EQ(h.type, d.type, "Type preserved");
    ASSERT_EQ(h.sec_hdr_flag, d.sec_hdr_flag, "Sec hdr flag preserved");
    ASSERT_EQ(h.apid, d.apid, "APID preserved");
    ASSERT_EQ(h.seq_flags, d.seq_flags, "Seq flags preserved");
    ASSERT_EQ(h.seq_count, d.seq_count, "Seq count preserved");
    ASSERT_EQ(h.length, d.length, "Length preserved");

    return 0;
}

int test_primary_pack_small_buffer_error(void)
{
    ccsds_primary_header_t h = make_primary_ok();
    uint8_t buf[5] = {0};
    size_t written = 0;
    ASSERT_EQ(CCSDS_ERR_LENGTH, pack_ccsds_primary_header(buf, sizeof(buf), &h, &written),
              "Primary pack fails when buf_len < CCSDS_MAX_PACKET_SIZE");
    return 0;
}

