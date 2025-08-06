#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include "ccsds_errors.h"
#include "ccsds.h"
#include "ccsds_header.h"
#include "ccsds_print.h"
#include "ccsds_encode.h"
#include "ccsds_types.h"
#include "command_registry.h"
#define CCSDS_PRIMARY_HEADER_SIZE 6
#define CCSDS_SECONDARY_HEADER_CUC_TIME_SIZE 8
#define CCSDS_SECONDARY_HEADER_TC_PUS_SIZE 3

int validate_args(int argc, char *argv[])
{
    if(argc < 2) {
        printf("Please pass a command for secondary header\n");
        return EXIT_FAILURE;
    } 
    
    ccsds_sec_header_types_t cmd = parse_command(argv[1]);
    if (cmd == CUC_TIME)
    {
        if(argc < 4 || argc > 4)
        {
            printf("Need only coarse time and fine time arguments when sending CUC_TIME secondary header cmd\n");
            return EXIT_FAILURE;
        }
    }
    else if (cmd == TC_PUS)
    {
        if(argc < 5 || argc > 5)
        {
            printf("Need only  arguments when sending TC_PUS secondary header cmd\n");
            return EXIT_FAILURE;
        }
    }
    return CCSDS_OK;
}

int validate_build_sec_header(ccsds_secondary_header_t *sec_header, int argc, char *argv[])
{
    ccsds_sec_header_types_t cmd = parse_command(argv[1]);

    if (cmd == CMD_NONE) {
        printf("(CUC_TIME, TC_PUS) are the only valid cmds at the moment\n");
        return EXIT_FAILURE;
    }


    // Assign type directly based on enum
    sec_header->type = (cmd == CUC_TIME) ? CCSDS_SEC_CUC_TIME : CCSDS_SEC_TC_PUS;

    ccsds_error_t result = build_secondary_header(sec_header, argc, argv);
    if (result != CCSDS_OK) {
        printf("Error building secondary header\n");
        return EXIT_FAILURE;
    }

    if (result != CCSDS_OK)
    {
        printError(result);
        return result;
    }

    return CCSDS_OK;
}

int main(int argc, char *argv[])
{
    if(validate_args(argc, argv) != 0)
    {
        return EXIT_FAILURE;
    }

    uint8_t *raw;
    size_t raw_size;

    const CommandDefinition* cmd = find_command_by_name(argv[1]);
    if(!cmd) {
        printf("This command is not allowed: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    ccsds_primary_header_t header;
    int32_t version = 0;
    
    // FIND A WAY TO TRACK THIS, wraps at 16383, 14 bit counter
    int32_t seq_count = 0;

    int32_t length;
    if(cmd->sec_hdr_flag == 1){
        length = get_secondary_header_size(cmd->sec_type) + cmd->static_payload_len_bytes - 1;
    }
    else 
    {
        length = cmd->static_payload_len_bytes - 1;
    }

    // Validate and build primary header
    ccsds_error_t result = build_primary_header(&header, version, cmd->type, cmd->sec_hdr_flag, cmd->apid, cmd->seq_flags, seq_count, length);
    if (result != CCSDS_OK)
    {
        printError(result);
        return result;
    }

    // Validate and build secondary header if flag is set
    ccsds_secondary_header_t sec_header = {0};
    size_t sec_header_len;
    if(header.sec_hdr_flag == 1)
    {
        validate_build_sec_header(&sec_header, argc, argv);

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
    else {
        sec_header_len = 0;
        raw_size = CCSDS_PRIMARY_HEADER_SIZE;
        raw = malloc(raw_size);
        if (!raw)
        {
            perror("malloc failed");
            return 1;
        }        
    }

    size_t bytes_written = encode_ccsds_packet(raw, &header, &sec_header, sec_header_len, NULL, 0);

    print_packet_summary(raw, bytes_written, &sec_header);
    free(raw);
    return 0;
}
