#include <stdio.h>

/* Primary header tests */
int test_invalid_primary_header(void);
int test_valid_primary_header_and_bytes(void);
int test_primary_pack_small_buffer_error(void);

/* Secondary header tests */
int test_secondary_header_cuc_time(void);
int test_secondary_header_tc_pus(void);
int test_secondary_header_builder_errors(void);

int main(void)
{
    printf("Running CCSDS tests...\n\n");

    if (test_invalid_primary_header()) return 1;
    if (test_valid_primary_header_and_bytes()) return 1;
    if (test_primary_pack_small_buffer_error()) return 1;

    if (test_secondary_header_cuc_time()) return 1;
    if (test_secondary_header_tc_pus()) return 1;
    if (test_secondary_header_builder_errors()) return 1;

    printf("\nAll tests passed!\n");
    return 0;
}

