#include "ccsds_types.h"
#include <stdlib.h>
#include "ccsds_header.h"

size_t get_secondary_header_size(ccsds_sec_type_t type) {
    switch (type) {
        case CCSDS_SEC_NONE:
            return 0;

        case CCSDS_SEC_CUC_TIME:
            return sizeof(((ccsds_secondary_header_t*)0)->data.cuc_time);
        
        case CCSDS_SEC_TC_PUS:
            return sizeof(((ccsds_secondary_header_t*)0)->data.tc_pus);
        
        default:
            return 0; // or error
    }
}


ccsds_error_t build_primary_header(
    ccsds_primary_header_t *hdr,
    int32_t version,
    int32_t type,         // 0 for telemetry, 1 for telecommand
    int32_t sec_hdr_flag, // 1 if Secondary Header is present, 0 if not
    int32_t apid,
    int32_t seq_flags, // 00 if the space packet(SP) contains a continuation segment of User Data, 01 if SP contains first segment of User Data, 10 if SP contains last packet, 11 if SP contains unsegmented data
    int32_t seq_count,
    int32_t length)
{
    // Validate each field BEFORE writing into the struct
    if (version < 0 || version > 7)
        return CCSDS_ERR_VERSION;
    if (type < 0 || type > 1)
        return CCSDS_ERR_TYPE;
    if (sec_hdr_flag < 0 || sec_hdr_flag > 1)
        return CCSDS_ERR_SEC_HDR_FLAG;
    if (apid < 0 || apid > 2047)
        return CCSDS_ERR_APID;
    if (seq_flags < 0 || seq_flags > 3)
        return CCSDS_ERR_SEQ_FLAGS;
    if (seq_count < 0 || seq_count > 16383)
        return CCSDS_ERR_SEQ_COUNT;
    if (length < 0 || length > 65535)
        return CCSDS_ERR_LENGTH;

    // Only write to struct if all values are valid
    hdr->version = (uint8_t)version;
    hdr->type = (uint8_t)type;
    hdr->sec_hdr_flag = (uint8_t)sec_hdr_flag;
    hdr->apid = (uint16_t)apid;
    hdr->seq_flags = (uint8_t)seq_flags;
    hdr->seq_count = (uint16_t)seq_count;
    hdr->length = (uint16_t)length;

    return CCSDS_OK;
}

ccsds_error_t build_secondary_header(ccsds_secondary_header_t *hdr,
                                     int argc, char **argv) {
    if (!hdr) return CCSDS_ERR_INVALID_ARG;

    switch (hdr->type) {
        case CCSDS_SEC_CUC_TIME:
            if (argc != 4) return CCSDS_ERR_INVALID_ARG;
            hdr->data.cuc_time.coarse_time = (uint32_t) strtoul(argv[2], NULL, 10);
            hdr->data.cuc_time.fine_time   = (uint32_t) strtoul(argv[3], NULL, 10);
            return CCSDS_OK;

        case CCSDS_SEC_TC_PUS:
            if (argc != 5) return CCSDS_ERR_INVALID_ARG;
            hdr->data.tc_pus.function_code = (uint8_t) strtoul(argv[2], NULL, 10);
            hdr->data.tc_pus.checksum      = (uint8_t) strtoul(argv[3], NULL, 10);
            hdr->data.tc_pus.spare         = (uint8_t) strtoul(argv[4], NULL, 10);
            return CCSDS_OK;

        default:
            return CCSDS_ERR_UNKNOWN_SEC_HDR_TYPE;
    }
}

