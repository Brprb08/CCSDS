#include <stdio.h>
#include <inttypes.h>
#include "ccsds_types.h"
#include "ccsds_packing.h"

void print_encode_decode(uint8_t *raw, size_t bytes_written, ccsds_secondary_header_t *sec_header)
{
    printf("Encoded header bytes: ");
    for (size_t i = 0; i < bytes_written; i++)
        printf("%02X ", raw[i]);
    printf("\n");

    // Decode back
    ccsds_primary_header_t decoded_primary;
    unpack_ccsds_primary_header(raw, &decoded_primary);

    printf("Decoded Primary:\n");
    printf("  Version: %d\n", decoded_primary.version);
    printf("  Type: %d\n", decoded_primary.type);
    printf("  Sec Header Flag: %d\n", decoded_primary.sec_hdr_flag);
    printf("  APID: %d\n", decoded_primary.apid);
    printf("  Sequence Flags: %d\n", decoded_primary.seq_flags);
    printf("  Sequence Count: %d\n", decoded_primary.seq_count);
    printf("  Length: %d (payload bytes: %d)\n\n",
           decoded_primary.length, decoded_primary.length + 1);

    // Decode back secondary header if present
    if (decoded_primary.sec_hdr_flag) {
        ccsds_secondary_header_t decoded_secondary;
        decoded_secondary.type = sec_header->type;  // pass along the same type used during encoding

        size_t r = unpack_ccsds_secondary_header(raw, &decoded_secondary);
        printf("Return from unpack secondary header: %ld\n", r);

        printf("Decoded Secondary:\n");

        switch (decoded_secondary.type) {
            case CCSDS_SEC_CUC_TIME:
                printf("  Coarse Time: %" PRIu32 "\n",
                       decoded_secondary.data.cuc_time.coarse_time);
                printf("  Fine Time: %" PRIu32 "\n",
                       decoded_secondary.data.cuc_time.fine_time);
                break;

            case CCSDS_SEC_TC_PUS:
                printf("  Function Code: %u\n",
                       decoded_secondary.data.tc_pus.function_code);
                printf("  Checksum: %u\n",
                       decoded_secondary.data.tc_pus.checksum);
                printf("  Spare: %u\n",
                       decoded_secondary.data.tc_pus.spare);
                break;

            default:
                printf("  Unknown secondary header type.\n");
                break;
        }
    }
}

