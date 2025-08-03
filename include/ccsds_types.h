#ifndef CCSDS_TYPES_H
#define CCSDS_TYPES_H

#include <stdint.h>
#include <stddef.h>

// ENUMS
typedef enum {
    CCSDS_OK = 0,
    CCSDS_ERR_VERSION,
    CCSDS_ERR_TYPE,
    CCSDS_ERR_SEC_HDR_FLAG,
    CCSDS_ERR_APID,
    CCSDS_ERR_SEQ_FLAGS,
    CCSDS_ERR_SEQ_COUNT,
    CCSDS_ERR_LENGTH,
    CCSDS_ERR_INVALID_ARG,
    CCSDS_ERR_UNKNOWN_SEC_HDR_TYPE,
} ccsds_error_t;

typedef enum {
    CCSDS_SEC_NONE = 0,
    CCSDS_SEC_CUC_TIME,
    CCSDS_SEC_TC_PUS
} ccsds_sec_type_t;

typedef enum {
    CUC_TIME,
    TC_PUS,
    CMD_NONE
} ccsds_sec_header_types_t;

// STRUCTS
typedef struct {
    uint8_t version;
    uint8_t type;
    uint8_t sec_hdr_flag;
    uint16_t apid;
    uint8_t seq_flags;
    uint16_t seq_count;
    uint16_t length;
} ccsds_primary_header_t;

typedef struct {
    ccsds_sec_type_t type;
    union {
        struct {
            uint32_t coarse_time;
            uint32_t fine_time;
        } cuc_time;

        struct {
            uint8_t function_code;
            uint8_t checksum;
            uint8_t spare;
        } tc_pus;
    } data;
} ccsds_secondary_header_t;

#endif // CCSDS_TYPES_H
