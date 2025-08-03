#include "ccsds.h"
#include <string.h>
#include <stddef.h>
#include "ccsds_types.h"

ccsds_sec_header_types_t parse_command(const char *cmd_str) {
    if (strcmp(cmd_str, "CUC_TIME") == 0) return CUC_TIME;
    if (strcmp(cmd_str, "TC_PUS") == 0)   return TC_PUS;
    return CMD_NONE;
}
