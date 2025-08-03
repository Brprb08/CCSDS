#ifndef CCSDS_HEADER_H
#define CCSDS_HEADER_H
#include "ccsds_types.h"

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

#endif
