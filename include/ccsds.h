#ifndef CCSDS_H
#define CCSDS_H

#include <stdint.h>
#include <stdint.h>
#include <stddef.h>

/*
 * CCSDS Primary Header structure
 * Mirrors the CCSDS 133.0-B-2 spec (6 bytes)
 */
typedef struct
{
    uint8_t version;      // 3 bits (should usually be 0)
    uint8_t type;         // 1 bit  (0=TM, 1=TC)
    uint8_t sec_hdr_flag; // 1 bit  (1=Secondary Header present)
    uint16_t apid;        // 11 bits (0-2047) maps to a payload struct

    uint8_t seq_flags;  // 2 bits (0-3)
    uint16_t seq_count; // 14 bits (0-16383)

    uint16_t length; // 16 bits (#bytes after header - 1)
} ccsds_primary_header_t;

typedef enum
{
	CCSDS_SEC_NONE = 0,
	CCSDS_SEC_CUC_TIME, // CCSDS Unsegmented Time Code (8 bytes: 4 coarse, 4 fine)
	CCSDS_SEC_TC_PUS // ESA PUS Telecommand header (3 bytes: function, checksum, spare)
} ccsds_sec_type_t;

typedef struct
{
	ccsds_sec_type_t type;

	union
	{
		struct
		{
			uint32_t coarse_time;
			uint32_t fine_time;
		} cuc_time;
		
		struct
		{
			uint8_t function_code; // Tells spacecraft what specific function the telecommand should trigger (0-255) (0-63)typically for "standard" missions
			uint8_t checksum; // Computed checksome value (0-255)
			uint8_t spare; // Almost always 0. Made for future specs or packet alignment
		} tc_pus;

	} data;
} ccsds_secondary_header_t;

//typedef struct
//{
//    uint32_t coarse_time; // Time since last epoch in seconds (typically Jan 1, 1958) 4 bytes
//    uint32_t fine_time;   // Fractional time from coarse time. Ex. If course time is not exactly right on the second, fine time gives the .5s after. So 80 00 00 00 = 2,147,483,648 / 4,294,967,296 (2^32) = .5
//} ccsds_secondary_header_t;

/*
 * Error codes for CCSDS operations
 */
typedef enum {
    CCSDS_OK = 0,
    CCSDS_ERR_VERSION,
    CCSDS_ERR_TYPE,
    CCSDS_ERR_SEC_HDR_FLAG,
    CCSDS_ERR_APID,
    CCSDS_ERR_SEQ_FLAGS,
    CCSDS_ERR_SEQ_COUNT,
    CCSDS_ERR_LENGTH,
    CCSDS_ERR_INVALID_ARG,          // NEW: For general bad arguments
    CCSDS_ERR_UNKNOWN_SEC_HDR_TYPE, // NEW: For unsupported secondary header
} ccsds_error_t;

/* Function declarations */
void printError(ccsds_error_t error);

ccsds_error_t build_primary_header(
    ccsds_primary_header_t *hdr,
    int32_t version,
    int32_t type,
    int32_t sec_hdr_flag,
    int32_t apid,
    int32_t seq_flags,
    int32_t seq_count,
    int32_t length);

ccsds_error_t build_secondary_header(
    ccsds_secondary_header_t *hdr,
    int argc,
    char **argv);

ccsds_error_t validateEncodePacket(ccsds_primary_header_t *h);

ccsds_error_t pack_ccsds_primary_header(uint8_t *buf, const ccsds_primary_header_t *h);

ccsds_error_t pack_ccsds_secondary_header(uint8_t *buf, const ccsds_secondary_header_t *h);

size_t encode_ccsds_packet(uint8_t *buf,
                           const ccsds_primary_header_t *h,
                           const ccsds_secondary_header_t *sh, size_t sec_len,
                           const uint8_t *payload, size_t payload_len);

void unpack_ccsds_primary_header(const uint8_t *buf, ccsds_primary_header_t *h);

size_t unpack_ccsds_secondary_header(const uint8_t *buf, ccsds_secondary_header_t *h);

#endif // CCSDS_H
