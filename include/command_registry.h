#ifndef COMMAND_REGISTRY_H
#define COMMAND_REGISTRY_H
#include "ccsds_types.h"

const CommandDefinition* find_command_by_name(const char* name);

#endif
