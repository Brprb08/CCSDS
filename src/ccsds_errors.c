#include "ccsds_errors.h"
#include <stdio.h>
#include "ccsds_types.h"

void printError(ccsds_error_t error)
{
    switch (error)
    {
    case CCSDS_ERR_VERSION:
        printf("Version is invalid. Must be 0–7.\n");
        break;
    case CCSDS_ERR_TYPE:
        printf("Type is invalid. Must be 0 or 1.\n");
        break;
    case CCSDS_ERR_SEC_HDR_FLAG:
        printf("Secondary Header Flag is invalid. Must be 0 or 1.\n");
        break;
    case CCSDS_ERR_APID:
        printf("APID is invalid. Must be 0–2047.\n");
        break;
    case CCSDS_ERR_SEQ_FLAGS:
        printf("Sequence Flags are invalid. Must be 0–3.\n");
        break;
    case CCSDS_ERR_SEQ_COUNT:
        printf("Sequence Count is invalid. Must be 0–16383.\n");
        break;
    case CCSDS_ERR_LENGTH:
        printf("Length is invalid. Must be 0–65535.\n");
        break;
    default:
        printf("Unknown error code: %d\n", error);
        break;
    }
}

