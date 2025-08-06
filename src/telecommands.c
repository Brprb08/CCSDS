#include "telecommands.h"
#include "ccsds_types.h"
#include <string.h>

// PAYLOAD FOR TC_PUS
static const TC_ParameterDef tc_pus_set_heater_params[] = {
    { "ZoneID",  0, 8,  "uint8",  NULL },
    { "State",   8, 8,  "uint8",  NULL },
    { "Timeout", 16, 16, "uint16", NULL }
};

const CommandDefinition command_defs[] = {
    {
        .name = "TC_PUS",
        .apid = 1,
        .type = 1,
        .sec_hdr_flag = 1,
        .sec_type = CCSDS_SEC_TC_PUS,
        .seq_flags = 3,
        .service_type = 1,
        .service_subtype = 2,
        .parameters = tc_pus_set_heater_params,
        .num_parameters = 3,
        .static_payload_len_bytes = 4
    },
    {
        .name = "CUC_TIME",
        .apid = 2,
        .type = 1,
        .sec_hdr_flag = 1,
        .sec_type = CCSDS_SEC_CUC_TIME,
        .seq_flags = 3,
        .parameters = NULL,
        .num_parameters = 0,
        .static_payload_len_bytes = 0
    },
};

const size_t command_def_count = sizeof(command_defs) / sizeof(command_defs[0]);
// More commands below...

