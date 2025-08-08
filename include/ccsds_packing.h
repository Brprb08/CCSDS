#ifndef CCSDS_PACKING_H
#define CCSDS_PACKING_H
#include "ccsds_types.h"

ccsds_error_t pack_ccsds_primary_header(uint8_t *buf, size_t buf_len, const ccsds_primary_header_t *h, size_t *out_len);
    
ccsds_error_t pack_ccsds_secondary_header(const uint8_t *buf, size_t buf_len, const ccsds_secondary_header_t *h, size_t *out_len);

void unpack_ccsds_primary_header(const uint8_t *buf, ccsds_primary_header_t *h);

size_t unpack_ccsds_secondary_header(const uint8_t *buf, ccsds_secondary_header_t *h);

#endif
