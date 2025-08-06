#include <string.h>
#include "ccsds_types.h"
#include "command_registry.h"
#include "telecommands.h"

const CommandDefinition* find_command_by_name(const char* name) {
    if (!name) {
        return NULL;
    }

    for (size_t i = 0; i < command_def_count; ++i) {
        if (command_defs[i].name && strcmp(command_defs[i].name, name) == 0) {
            return &command_defs[i];
        }
    }

    return NULL;
}

