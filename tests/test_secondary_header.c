#include "test_helpers.h"

int test_secondary_header_cuc_time(void)
{
    int argc = 4;
    char *argv[] = {"program", "CUC_TIME", "1753658202", "1288490188"};
    ccsds_secondary_header_t sh = { .type = CCSDS_SEC_CUC_TIME };

    ASSERT_EQ(CCSDS_OK, build_secondary_header(&sh, argc, argv),
              "CUC_TIME builder accepts valid input");

    uint8_t secbuf[16] = {0};
    size_t sec_written = 0;
    ASSERT_EQ(8, pack_ccsds_secondary_header(secbuf, sizeof(secbuf), &sh, &sec_written),
              "CUC_TIME pack returns 8");
    ASSERT_EQ(8, sec_written, "CUC_TIME out_len is 8");

    uint8_t expected[8] = { 0x68, 0x86, 0xB3, 0x5A, 0x4C, 0xCC, 0xCC, 0xCC };
    ASSERT_MEMEQ(expected, secbuf, 8, "CUC_TIME bytes match");

    uint8_t full[6 + 8] = {0};
    memcpy(full + 6, secbuf, 8);
    ccsds_secondary_header_t out = { .type = CCSDS_SEC_CUC_TIME };
    ASSERT_EQ(6, unpack_ccsds_secondary_header(full, &out),
              "CUC_TIME unpack returns 6 (impl quirk)");
    ASSERT_EQ(sh.data.cuc_time.coarse_time, out.data.cuc_time.coarse_time, "CUC coarse preserved");
    ASSERT_EQ(sh.data.cuc_time.fine_time, out.data.cuc_time.fine_time, "CUC fine preserved");

    return 0;
}

int test_secondary_header_tc_pus(void)
{
    int argc = 5;
    char *argv[] = {"program", "TC_PUS", "12", "34", "56"};
    ccsds_secondary_header_t sh = { .type = CCSDS_SEC_TC_PUS };

    ASSERT_EQ(CCSDS_OK, build_secondary_header(&sh, argc, argv),
              "TC_PUS builder accepts valid input");

    uint8_t secbuf[8] = {0};
    size_t sec_written = 0;
    ASSERT_EQ(3, pack_ccsds_secondary_header(secbuf, sizeof(secbuf), &sh, &sec_written),
              "TC_PUS pack returns 3");
    ASSERT_EQ(3, sec_written, "TC_PUS out_len is 3");

    uint8_t expected[3] = { 12, 34, 56 };
    ASSERT_MEMEQ(expected, secbuf, 3, "TC_PUS bytes match");

    uint8_t full[6 + 3] = {0};
    memcpy(full + 6, secbuf, 3);
    ccsds_secondary_header_t out = { .type = CCSDS_SEC_TC_PUS };
    ASSERT_EQ(3, unpack_ccsds_secondary_header(full, &out),
              "TC_PUS unpack returns 3");
    ASSERT_EQ(sh.data.tc_pus.function_code, out.data.tc_pus.function_code, "Function preserved");
    ASSERT_EQ(sh.data.tc_pus.checksum, out.data.tc_pus.checksum, "Checksum preserved");
    ASSERT_EQ(sh.data.tc_pus.spare, out.data.tc_pus.spare, "Spare preserved");

    return 0;
}

int test_secondary_header_builder_errors(void)
{
    ccsds_secondary_header_t sh = {0};

    sh.type = CCSDS_SEC_CUC_TIME;
    { int argc = 3; char *argv[] = {"program", "CUC_TIME", "1753658202"};
      ASSERT_EQ(CCSDS_ERR_INVALID_ARG, build_secondary_header(&sh, argc, argv),
                "CUC_TIME builder fails on argc != 4"); }

    sh.type = CCSDS_SEC_TC_PUS;
    { int argc = 4; char *argv[] = {"program", "TC_PUS", "1", "2"};
      ASSERT_EQ(CCSDS_ERR_INVALID_ARG, build_secondary_header(&sh, argc, argv),
                "TC_PUS builder fails on argc != 5"); }

    sh.type = 0xFF;
    { int argc = 1; char *argv[] = {"program"};
      ASSERT_EQ(CCSDS_ERR_UNKNOWN_SEC_HDR_TYPE, build_secondary_header(&sh, argc, argv),
                "Builder fails on unknown type"); }

    return 0;
}

