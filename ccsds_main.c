/*
 * CCSDS Space Packet Protocol - Primary Header Example
 * Compliant with CCSDS 133.0-B-2 (June 2020)
 *
 * Primary Header: 6 bytes (48 bits)
 *  Bits 0-2:   Version Number (3 bits)
 *  Bit 3:      Packet Type (1 bit) 0=Telemetry, 1=Telecommand
 *  Bit 4:      Secondary Header Flag (1 bit)
 *  Bits 5-15:  APID (11 bits)
 *
 *  Bits 16-17: Sequence Flags (2 bits)
 *  Bits 18-31: Packet Sequence Count (14 bits)
 *
 *  Bits 32-47: Packet Data Length (16 bits)
 *              (Equals number of bytes after this header - 1)
 */

#include <stdio.h>
#include <stdint.h>

typedef struct
{
    uint8_t version;      // 3 bits (should usually be 0)
    uint8_t type;         // 1 bit  (0=TM, 1=TC)
    uint8_t sec_hdr_flag; // 1 bit  (1=Secondary Header present)
    uint16_t apid;        // 11 bits (0-2047 valid APIDs)

    uint8_t seq_flags;  // 2 bits (11=unsegmented, etc.)
    uint16_t seq_count; // 14 bits (0-16383)

    uint16_t length; // 16 bits (#bytes after header - 1)
} ccsds_primary_header_t;

typedef enum
{
    CCSDS_OK = 0,           // success
    CCSDS_ERR_VERSION = -1, // version out of range
    CCSDS_ERR_TYPE = -2,    // type out of range
    CCSDS_ERR_SEC_HDR_FLAG = -3,
    CCSDS_ERR_APID = -4,      // APID out of range
    CCSDS_ERR_SEQ_FLAGS = -5, // seq_flags out of range
    CCSDS_ERR_SEQ_COUNT = -6, // seq_count out of range
    CCSDS_ERR_LENGTH = -7     // length out of range
} ccsds_error_t;

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

ccsds_error_t validateEncodePacket(ccsds_primary_header_t *h)
{
    if (h->version > 7)
    {
        return CCSDS_ERR_VERSION;
    }

    if (h->type > 1)
    {
        return CCSDS_ERR_TYPE;
    }

    if (h->sec_hdr_flag > 1)
    {
        return CCSDS_ERR_SEC_HDR_FLAG;
    }

    if (h->apid > 2047)
    {
        return CCSDS_ERR_APID;
    }

    if (h->seq_flags > 3)
    {
        return CCSDS_ERR_SEQ_FLAGS;
    }

    if (h->seq_count > 16383)
    {
        return CCSDS_ERR_SEQ_COUNT;
    }

    if (h->length > 65535)
    {
        return CCSDS_ERR_LENGTH;
    }

    return CCSDS_OK;
}

/*
 * Encode CCSDS primary header into 6 raw bytes
 */
int pack_ccsds_header(uint8_t *buf, const ccsds_primary_header_t *h)
{
    // EACH BUFFER IS 1 byte, 8 bits

    // FLOW
    // MASK to keep the bits you want (&)
    // SHIFT to move the bits to the right spot (<<, >>)
    // OR the pieces together to 'stack' them in one byte

    // Byte 0: version (3 bits), type (1), sec hdr (1), upper 3 bits of APID
    // Rundown
    // 0x07 = 0000 0111 so we mask the last three bits
    // if version = 2 -> 2 = 0000 0010 -> we keep just 010
    // 0x01 = 0000 0001 so we mask the last bit
    // if type = 1 -> 1 = 0000 0001 -> we keep just 1
    // << = shift left -> say we have: 0000 0010 << 5, left shift of 5 = 0100 0000
    // >> = shift right -> say we have 010 0010 0000, right shift 8 = 000 0000 0010
    // For apid we save first three bits of 11 bit number into buf[0], thats why we have this: (h->apid >> 8) & 0x07)

    // Byte Layout:
    //  Byte 0: version (3) | type (1) | sec_hdr (1) | APID[10:8] (3)
    //  Byte 1: APID[7:0]
    //  Byte 2: seq_flags (2) | seq_count[13:8] (6)
    //  Byte 3: seq_count[7:0]
    //  Byte 4: length[15:8]
    //  Byte 5: length[7:0]

    buf[0] = ((h->version & 0x07) << 5) |
             ((h->type & 0x01) << 4) |
             ((h->sec_hdr_flag & 0x01) << 3) |
             ((h->apid >> 8) & 0x07);

    // Byte 1: lower 8 bits of APID
    buf[1] = h->apid & 0xFF;

    // Byte 2: seq flags (2 bits), upper 6 bits of seq_count
    buf[2] = ((h->seq_flags & 0x03) << 6) |
             ((h->seq_count >> 8) & 0x3F);
    // Say seq count = 42, in binary for 14 bit max this = 00 0000 0010 1010
    // shifted 8 bits = 00 0000 0000 0000
    // mask: 0011 1111 saves upper 6 bits -> here it saves 00 0000

    // Byte 3: lower 8 bits of seq_count
    buf[3] = h->seq_count & 0xFF;

    // Bytes 4-5: Packet length
    buf[4] = (h->length >> 8) & 0xFF;
    buf[5] = h->length & 0xFF;

    return 0;
}

/*
 * Decode 6 raw bytes into CCSDS primary header struct
 */
void unpack_ccsds_header(const uint8_t *buf, ccsds_primary_header_t *h)
{
    h->version = (buf[0] >> 5) & 0x07;
    h->type = (buf[0] >> 4) & 0x01;
    h->sec_hdr_flag = (buf[0] >> 3) & 0x01;
    h->apid = ((buf[0] & 0x07) << 8) | buf[1];

    h->seq_flags = (buf[2] >> 6) & 0x03;
    h->seq_count = ((buf[2] & 0x3F) << 8) | buf[3];

    h->length = ((uint16_t)buf[4] << 8) | buf[5];
}

/*
 * Example usage
 */
int main(void)
{
    // Build a header in struct form
    // .variableName = designated initilizer
    // Lets you initialize specific struct fields by name rather than position
    // Avoids mistakes if someone changes the struct later (adds new field)
    // Without it you would need to initialize the struct in the exact order of struct definition
    ccsds_primary_header_t header = {
        .version = 0,      // 3 bits: min 0, max 7   (CCSDS spec always uses 0)
        .type = 1,         // 1 bit:  min 0, max 1   (0=Telemetry, 1=Telecommand)
        .sec_hdr_flag = 1, // 1 bit:  min 0, max 1   (1=Secondary header present)
        .apid = 100,       // 11 bits: min 0, max 2047 (Application Process ID)
        .seq_flags = 2,    // 2 bits:  min 0, max 3   (3='11' = unsegmented)
        .seq_count = 42,   // 14 bits: min 0, max 16383 (wraps around after max)
        .length = 15       // 16 bits: min 0, max 65535 (payload = length+1 bytes)
    };

    uint8_t raw[6];

    // Valiation
    ccsds_error_t validateEncodePacketResult = validateEncodePacket(&header);
    if (validateEncodePacketResult != CCSDS_OK)
    {
        printError(validateEncodePacketResult);
        return validateEncodePacketResult;
    }

    // Encode
    ccsds_error_t encodeResult = pack_ccsds_header(raw, &header);
    if (encodeResult != 0)
    {
        printf("Error encoding packet. Stopping.");
    }

    printf("Encoded header bytes: ");
    for (int i = 0; i < 6; i++)
    {
        printf("%02X ", raw[i]);
    }
    printf("\n");

    // Decode back from raw bytes to struct
    ccsds_primary_header_t decoded;
    unpack_ccsds_header(raw, &decoded);

    printf("Decoded:\n");
    printf("  Version: %d\n", decoded.version);
    printf("  Type: %d\n", decoded.type);
    printf("  Secondary Header Flag: %d\n", decoded.sec_hdr_flag);
    printf("  APID: %d\n", decoded.apid);
    printf("  Sequence Flags: %d\n", decoded.seq_flags);
    printf("  Sequence Count: %d\n", decoded.seq_count);
    printf("  Length: %d (payload bytes: %d)\n",
           decoded.length, decoded.length + 1);

    return 0;
}
