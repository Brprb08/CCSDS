#include "ccsds_types.h"
#include "ccsds_packing.h"
#include <string.h>

size_t encode_ccsds_packet(uint8_t *buf,
                           const ccsds_primary_header_t *h,
                           const ccsds_secondary_header_t *sh, size_t sec_len,
                           const uint8_t *payload, size_t payload_len)
{
    pack_ccsds_primary_header(buf, h);
    size_t offset = 6;

    // 2. Optional: secondary header
    if (h->sec_hdr_flag && sh && sec_len > 0)
    {
        // uint8_t raw_secondary[8];
        pack_ccsds_secondary_header(buf, sh);
        offset += sec_len;
    }

    // 3. Optional: payload
    if (payload && payload_len > 0)
    {
        memcpy(buf + offset, payload, payload_len);
        offset += payload_len;
    }

    return offset;
}

