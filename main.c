#include <stdio.h>
#include "ccsds.h"
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#define CCSDS_PRIMARY_HEADER_SIZE 6
#define CCSDS_SECONDARY_HEADER_CUC_TIME_SIZE 8
#define CCSDS_SECONDARY_HEADER_TC_PUS_SIZE 3

int main(int argc, char *argv[])
{
    // EVENTUALLY VALIDATE THE ARGC and ARGV
    if(argc < 2) {
        printf("Please pass a command for secondary header\n");
        return EXIT_FAILURE;
    } 
    else if (strcmp(argv[1], "CUC_TIME") == 0)
    {
        if(argc < 4 || argc > 4)
        {
            printf("Need only coarse time and fine time arguments when sending CUC_TIME secondary header cmd\n");
            return EXIT_FAILURE;
        }
    }

    // else if here to check argv[1] later 

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
    ccsds_secondary_header_t sec_header = {0};
    int include_sec_hdr = 0;
    if (header.sec_hdr_flag == 1)
    {
        if(strcmp(argv[1], "CUC_TIME") == 0)
        {
            include_sec_hdr = 1;
            
            sec_header.type = CCSDS_SEC_CUC_TIME;
            ccsds_error_t result = build_secondary_header(&sec_header, argc, argv);
            if(result != 0)
            {
                printf("Error building secondary header\n");
                return EXIT_FAILURE;
            }
        } 
        else if(strcmp(argv[1], "TC_PUS") == 0)
        {
            include_sec_hdr = 1;

            sec_header.type = CCSDS_SEC_TC_PUS;
            ccsds_error_t result = build_secondary_header(&sec_header, argc, argv);
            if(result != 0)
            {
                printf("Error building secondary header\n");
                return EXIT_FAILURE;
            }
        }
        else 
        {
            printf("(CUC_TIME, TC_PUS) are the only valid cmds at the moment\n");
            return EXIT_FAILURE;
        }
        // uint32_t coarse = 1753658202; // 0110 1000 1000 0110 1011 0011 0101 1010 = hex(68 86 B3 5A)
        // uint32_t fine = 1288490188;   // 0100 1100 1100 1100 1100 1100 1100 1100 = hex(4C CC CC CC)
        //result = build_secondary_header(&sec_header, coarse, fine);
        if (result != CCSDS_OK)
        {
            printError(result);
            return result;
        }
    }

    size_t sec_header_len;
    // Maybe calloc later?
    if (include_sec_hdr)
    {
        switch(sec_header.type)
        {
            case(CCSDS_SEC_CUC_TIME):
                raw_size = CCSDS_PRIMARY_HEADER_SIZE + CCSDS_SECONDARY_HEADER_CUC_TIME_SIZE;
                sec_header_len = CCSDS_SECONDARY_HEADER_CUC_TIME_SIZE;
                break;
            case(CCSDS_SEC_TC_PUS):
                raw_size = CCSDS_PRIMARY_HEADER_SIZE + CCSDS_SECONDARY_HEADER_TC_PUS_SIZE;
                sec_header_len = CCSDS_SECONDARY_HEADER_TC_PUS_SIZE;
                break;
            default:
                raw_size = 0;
        }

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

    // Encode
    size_t bytes_written = encode_ccsds_packet(raw, &header, &sec_header, sec_header_len, NULL, 0);

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
        decoded_secondary.type = sec_header.type;  // pass along the same type used during encoding

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
    free(raw);
    return 0;
}
