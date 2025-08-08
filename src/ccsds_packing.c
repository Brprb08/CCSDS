#include "ccsds_types.h"
#include "ccsds_defs.h"
#include <stddef.h>
#include <stdio.h>
ccsds_error_t pack_ccsds_primary_header(uint8_t *buf, size_t buf_len, const ccsds_primary_header_t *h, size_t *out_len)
{
    if(buf_len < CCSDS_MAX_PACKET_SIZE){
        return CCSDS_ERR_LENGTH;
    }
    // Byte 0
    buf[0] = ((h->version & 0x07) << 5) |
             ((h->type & 0x01) << 4) |
             ((h->sec_hdr_flag & 0x01) << 3) |
             ((h->apid >> 8) & 0x07);

    // Byte 1: lower 8 bits of APID
    buf[1] = h->apid & 0xFF;

    // Byte 2: seq flags (2 bits), upper 6 bits of seq_count
    buf[2] = ((h->seq_flags & 0x03) << 6) |
             ((h->seq_count >> 8) & 0x3F);

    // Byte 3: lower 8 bits of seq_count
    buf[3] = h->seq_count & 0xFF;

    // Bytes 4–5: Packet length
    buf[4] = (h->length >> 8) & 0xFF;
    buf[5] = h->length & 0xFF;
    
    *out_len = CCSDS_PRIMARY_HEADER_SIZE;
    return CCSDS_OK;
}

// CUC time = CCSDS Unsegmented Time Code
// Fine time = .3 * 2^32 = 1288490188.8 -> truncated = 1288490188

ccsds_error_t pack_ccsds_secondary_header(uint8_t *buf, size_t buf_len, const ccsds_secondary_header_t *h, size_t *out_len)
{
    // This if avoids a warning of buf len not getting used remove later
    if(buf_len) {
        //do nothing
    }

    switch(h->type)
    {
	case(CCSDS_SEC_NONE):
	    *out_len = 0;
        return 0;
	case(CCSDS_SEC_CUC_TIME):
        buf[0] = (h->data.cuc_time.coarse_time >> 24) & 0xFF;
	    buf[1] = (h->data.cuc_time.coarse_time >> 16) & 0xFF;
	    buf[2] = (h->data.cuc_time.coarse_time >> 8) & 0xFF;
	    buf[3] = h->data.cuc_time.coarse_time & 0xFF;

	    buf[4] = (h->data.cuc_time.fine_time >> 24) & 0xFF;
	    buf[5] = (h->data.cuc_time.fine_time >> 16) & 0xFF;
	    buf[6] = (h->data.cuc_time.fine_time >> 8) & 0xFF;
	    buf[7] = h->data.cuc_time.fine_time & 0xFF;
	    // Number of bytes
	    *out_len = 8;
        return 8;
	case(CCSDS_SEC_TC_PUS):
	    buf[0] = h->data.tc_pus.function_code;
	    buf[1] = h->data.tc_pus.checksum;
	    buf[2] = h->data.tc_pus.spare;
	    // Number of bytes
	    *out_len = 3;
        return 3;
    }
    return CCSDS_OK;
}

void unpack_ccsds_primary_header(const uint8_t *buf, ccsds_primary_header_t *h)
{
    h->version = (buf[0] >> 5) & 0x07;
    h->type = (buf[0] >> 4) & 0x01;
    h->sec_hdr_flag = (buf[0] >> 3) & 0x01;
    h->apid = ((buf[0] & 0x07) << 8) | buf[1];

    h->seq_flags = (buf[2] >> 6) & 0x03;
    h->seq_count = ((buf[2] & 0x3F) << 8) | buf[3];

    h->length = ((uint16_t)buf[4] << 8) | buf[5];
}

size_t unpack_ccsds_secondary_header(const uint8_t *buf, ccsds_secondary_header_t *h)
{
    switch(h->type)
    {
	case(CCSDS_SEC_NONE):
	    return 0;
	case(CCSDS_SEC_CUC_TIME):
	    h->data.cuc_time.coarse_time = ((uint32_t)buf[6] << 24) |
			     ((uint32_t)buf[7] << 16) |
			     ((uint32_t)buf[8] << 8) |
			     (uint32_t)buf[9];

	    h->data.cuc_time.fine_time = ((uint32_t)buf[10] << 24) |
			   ((uint32_t)buf[11] << 16) |
			   ((uint32_t)buf[12] << 8) |
			   (uint32_t)buf[13];
	    return 6;
	case(CCSDS_SEC_TC_PUS):
	    h->data.tc_pus.function_code = buf[6];
	    h->data.tc_pus.checksum = buf[7];
	    h->data.tc_pus.spare = buf[8];
	    return 3;
	default:
	    return CCSDS_ERR_UNKNOWN_SEC_HDR_TYPE;
    }
}

