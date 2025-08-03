#ifndef CCSDS_PRINT_H
#define CCSDS_PRINT_H
#include "ccsds_types.h"

void print_encode_decode(uint8_t *raw, size_t bytes_written, ccsds_secondary_header_t *sec_header);

void print_packet_summary(const uint8_t *raw, size_t bytes_written, const ccsds_secondary_header_t *sec_header);

#endif

