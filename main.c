#include <stdio.h>
#include "ccsds.h"
#include <inttypes.h>
#include <stdlib.h>
#define CCSDS_PRIMARY_HEADER_SIZE 6
#define CCSDS_SECONDARY_HEADER_SIZE 8

int main(void)
{
    ccsds_primary_header_t header;
    uint8_t *raw;
    size_t raw_size;

    int32_t version = 0;
    int32_t type = 1;
    int32_t sec_hdr_flag = 1;
    int32_t apid = 100;
    int32_t seq_flags = 2;
    int32_t seq_count = 42;
    int32_t length = 15;

    // Validate and build primary header
    ccsds_error_t result = build_primary_header(&header, version, type, sec_hdr_flag, apid, seq_flags, seq_count, length);
    if (result != CCSDS_OK)
    {
        printError(result);
        return result;
    }

    // Validate and build secondary header if flag is set
    ccsds_secondary_header_t sec_header;
    int include_sec_hdr = 0;
    if (header.sec_hdr_flag == 1)
    {
        include_sec_hdr = 1;

        uint32_t coarse = 1753658202; // 0110 1000 1000 0110 1011 0011 0101 1010 = hex(68 86 B3 5A)
        uint32_t fine = 1288490188;   // 0100 1100 1100 1100 1100 1100 1100 1100 = hex(4C CC CC CC)
        ccsds_error_t result = build_secondary_header(&sec_header, coarse, fine);
        if (result != CCSDS_OK)
        {
            printError(result);
            return result;
        }
    }

    if (include_sec_hdr)
    {
        raw_size = CCSDS_PRIMARY_HEADER_SIZE + CCSDS_SECONDARY_HEADER_SIZE;
        raw = malloc(raw_size);
        if (!raw)
        {
            perror("malloc failed");
            return 1;
        }
    }
    else
    {
        raw_size = CCSDS_PRIMARY_HEADER_SIZE;
        raw = malloc(raw_size);
        if (!raw)
        {
            perror("malloc failed");
            return 1;
        }
    }

    uint8_t sec_len = 8;

    // Encode
    size_t bytes_written = encode_ccsds_packet(raw, &header, &sec_header, sec_len, NULL, 0);

    printf("Encoded header bytes: ");
    for (size_t i = 0; i < bytes_written; i++)
        printf("%02X ", raw[i]);
    printf("\n");

    // Decode back
    ccsds_primary_header_t decoded_primary;
    unpack_ccsds_primary_header(raw, &decoded_primary);

    ccsds_secondary_header_t decoded_secondary;
    unpack_ccsds_secondary_header(raw, &decoded_secondary);

    printf("Decoded Primary:\n");
    printf("  Version: %d\n", decoded_primary.version);
    printf("  Type: %d\n", decoded_primary.type);
    printf("  Sec Header Flag: %d\n", decoded_primary.sec_hdr_flag);
    printf("  APID: %d\n", decoded_primary.apid);
    printf("  Sequence Flags: %d\n", decoded_primary.seq_flags);
    printf("  Sequence Count: %d\n", decoded_primary.seq_count);
    printf("  Length: %d (payload bytes: %d)\n\n",
           decoded_primary.length, decoded_primary.length + 1);

    printf("Decoded Secondary:\n");
    printf("  Coarse: %" PRIu32 "\n", decoded_secondary.coarse_time);
    printf("  Fine Time: %" PRId32 "\n", decoded_secondary.fine_time);

    free(raw);
    return 0;
}
