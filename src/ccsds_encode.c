#include "ccsds_types.h"
#include "ccsds_packing.h"
#include <string.h>
#include <stdio.h>
size_t encode_ccsds_packet(uint8_t *buf, size_t buf_len,
                                  const ccsds_primary_header_t *h,
                                  const ccsds_secondary_header_t *sh,
                                  const uint8_t *payload, size_t payload_len,
                                  size_t *packet_len_out)
{
    size_t offset = 0, written;
    ccsds_error_t err;

    // 1) Primary header
    err = pack_ccsds_primary_header(buf + offset, buf_len - offset, h, &written);
    if (err) return err;
    
    offset += written;

    // 2) Secondary header (if any)
    if (h->sec_hdr_flag) {
        err = pack_ccsds_secondary_header(buf + offset, buf_len - offset, sh, &written);
        offset += written;
    }

    // 3) Payload
    if (payload && payload_len > 0) {
        if (offset + payload_len > buf_len)
            return CCSDS_ERR_LENGTH;
        memcpy(buf + offset, payload, payload_len);
        offset += payload_len;
    }

    *packet_len_out = offset;
    return *packet_len_out;
}

