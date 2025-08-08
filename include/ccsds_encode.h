#ifndef CCSDS_ENCODE_H
#define CCSDS_ENCODE_H
#include "ccsds_types.h"

size_t encode_ccsds_packet(uint8_t *buf, size_t buf_len,
                                  const ccsds_primary_header_t *h,
                                  const ccsds_secondary_header_t *sh,
                                  const uint8_t *payload, size_t payload_len,
                                  size_t *packet_len_out);

#endif

