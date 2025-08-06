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
    uint8_t version;         // 3 bits: CCSDS version number (usually 0)
    uint8_t type;            // 1 bit: 0 = telemetry, 1 = telecommand
    uint8_t sec_hdr_flag;    // 1 bit: 1 = secondary header is present, 0 = not present

    uint16_t apid;           // 11 bits: Application Process ID — identifies the packet's source or destination application

    uint8_t seq_flags;       // 2 bits: Sequence flags
                             //   00 = continuation segment
                             //   01 = first segment
                             //   10 = last segment
                             //   11 = unsegmented (most common)

    uint16_t seq_count;      // 14 bits: Packet sequence count (incrementing counter per APID, wraps at 16383)

    uint16_t length;         // 16 bits: Packet length = (total_packet_length - 1 - 6)
                             // (does NOT include the 6-byte primary header itself)
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

// Represents a single parameter (field) in the packet
typedef struct {
    const char* name;        // Parameter name
    uint16_t bit_offset;     // Bit offset within the packet
    uint16_t bit_length;     // Length in bits
    const char* data_type;   // e.g., "uint8", "float32", etc.
    void* default_value;     // Pointer to default value (if any)
} TC_ParameterDef;

typedef struct {
    const char* name;                // Command name, e.g., "TC_PUS"
    uint16_t apid;                   // CCSDS APID
    uint8_t type;                    // 0 = telemetry, 1 = telecommand
    uint8_t sec_hdr_flag;           // 1 = secondary header present
    ccsds_sec_type_t sec_type;      // CCSDS_SEC_NONE / CCSDS_SEC_CUC_TIME / CCSDS_SEC_TC_PUS
    uint8_t seq_flags;              // Typically 3 (unsegmented)
    uint8_t service_type;           // PUS: service type
    uint8_t service_subtype;        // PUS: service subtype

    const ccsds_secondary_header_t* sec_hdr_template; // optional default values
    const TC_ParameterDef* parameters; // payload parameter layout
    uint16_t num_parameters;

    uint16_t static_payload_len_bytes; // optional, or calculated from parameters
} CommandDefinition;


#endif // CCSDS_TYPES_H
