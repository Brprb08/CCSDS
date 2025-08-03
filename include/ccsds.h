#ifndef CCSDS_H
#define CCSDS_H

#include <stdint.h>
#include <stdint.h>
#include <stddef.h>
#include "ccsds_types.h"

ccsds_sec_header_types_t parse_command(const char *argv);

#endif // CCSDS_H
