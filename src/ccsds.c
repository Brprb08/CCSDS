#include "ccsds.h"
#include <stdio.h>
#include <string.h>
#include <stddef.h>

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

ccsds_error_t build_primary_header(
    ccsds_primary_header_t *hdr,
    int32_t version,
    int32_t type,         // 0 for telemetry, 1 for telecommand
    int32_t sec_hdr_flag, // 1 if Secondary Header is present, 0 if not
    int32_t apid,
    int32_t seq_flags, // 00 if the space packet(SP) contains a continuation segment of User Data, 01 if SP contains first segment of User Data, 10 if SP contains last packet, 11 if SP contains unsegmented data
    int32_t seq_count,
    int32_t length)
{
    // Validate each field BEFORE writing into the struct
    if (version < 0 || version > 7)
        return CCSDS_ERR_VERSION;
    if (type < 0 || type > 1)
        return CCSDS_ERR_TYPE;
    if (sec_hdr_flag < 0 || sec_hdr_flag > 1)
        return CCSDS_ERR_SEC_HDR_FLAG;
    if (apid < 0 || apid > 2047)
        return CCSDS_ERR_APID;
    if (seq_flags < 0 || seq_flags > 3)
        return CCSDS_ERR_SEQ_FLAGS;
    if (seq_count < 0 || seq_count > 16383)
        return CCSDS_ERR_SEQ_COUNT;
    if (length < 0 || length > 65535)
        return CCSDS_ERR_LENGTH;

    // Only write to struct if all values are valid
    hdr->version = (uint8_t)version;
    hdr->type = (uint8_t)type;
    hdr->sec_hdr_flag = (uint8_t)sec_hdr_flag;
    hdr->apid = (uint16_t)apid;
    hdr->seq_flags = (uint8_t)seq_flags;
    hdr->seq_count = (uint16_t)seq_count;
    hdr->length = (uint16_t)length;

    return CCSDS_OK;
}

ccsds_error_t build_secondary_header(
    ccsds_secondary_header_t *hdr,
    uint32_t coarse,
    uint32_t fine)
{
    // Validate each field BEFORE writing into the struct
    if (coarse < 0)
        return CCSDS_ERR_COURSE;
    if (fine < 0)
        return CCSDS_ERR_FINE;

    // Only write to struct if all values are valid
    hdr->coarse_time = (uint32_t)coarse;
    hdr->fine_time = (uint32_t)fine;

    return CCSDS_OK;
}

ccsds_error_t pack_ccsds_primary_header(uint8_t *buf, const ccsds_primary_header_t *h)
{
    // Each element in `buf` represents **1 byte (8 bits)** of the CCSDS header.
    //
    // Bit-packing strategy:
    //  • MASK  – use & (bitwise AND) to isolate the bits you want
    //  • SHIFT – move those bits into the correct position using << or >>
    //  • OR    – (bitwise OR) combine fields together into the same byte using |
    //
    // ──────────────────────────────────────────────
    // Byte layout (matches CCSDS 133.0-B-2 spec):
    //   Byte 0: version (3 bits) | type (1) | sec_hdr_flag (1) | APID[10:8] (3)
    //   Byte 1: APID[7:0]
    //   Byte 2: seq_flags (2 bits) | seq_count[13:8] (6)
    //   Byte 3: seq_count[7:0]
    //   Byte 4: length[15:8]
    //   Byte 5: length[7:0]
    // ──────────────────────────────────────────────
    //
    // Example breakdown for Byte 0:
    //   • version → mask with 0x07 (0000 0111) to keep its 3 bits,
    //               then shift left 5 (<< 5) to place it in bits 7–5
    //   • type → mask with 0x01 (0000 0001), shift left 4 (<< 4)
    //   • sec_hdr_flag → mask with 0x01, shift left 3 (<< 3)
    //   • APID → take the top 3 bits (apid >> 8), mask with 0x07, and OR into byte
    //
    // General rule for all bytes: mask the field, shift into place, then OR
    // the pieces together to build the final 6-byte CCSDS Primary Header.

    // Byte 0
    buf[0] = ((h->version & 0x07) << 5) |
             ((h->type & 0x01) << 4) |
             ((h->sec_hdr_flag & 0x01) << 3) |
             ((h->apid >> 8) & 0x07);

    // Byte 1: lower 8 bits of APID
    buf[1] = h->apid & 0xFF;

    // Byte 2: seq flags (2 bits), upper 6 bits of seq_count
    buf[2] = ((h->seq_flags & 0x03) << 6) |
             ((h->seq_count >> 8) & 0x3F);

    // Byte 3: lower 8 bits of seq_count
    buf[3] = h->seq_count & 0xFF;

    // Bytes 4–5: Packet length
    buf[4] = (h->length >> 8) & 0xFF;
    buf[5] = h->length & 0xFF;

    return CCSDS_OK;
}

// CUC time = CCSDS Unsegmented Time Code
// Fine time = .3 * 2^32 = 1288490188.8 -> truncated = 1288490188
ccsds_error_t pack_ccsds_secondary_header(uint8_t *buf, const ccsds_secondary_header_t *h)
{
    buf[6] = (h->coarse_time >> 24) & 0xFF;
    buf[7] = (h->coarse_time >> 16) & 0xFF;
    buf[8] = (h->coarse_time >> 8) & 0xFF;
    buf[9] = h->coarse_time & 0xFF;

    buf[10] = (h->fine_time >> 24) & 0xFF;
    buf[11] = (h->fine_time >> 16) & 0xFF;
    buf[12] = (h->fine_time >> 8) & 0xFF;
    buf[13] = h->fine_time & 0xFF;

    return CCSDS_OK;
}

// ------------------------------------------------ TODO ------------------------------------------------------
// Secondary header can include timestamp (common for telemetry), a command header(function code, checksum, ...), mission specific info(mode indicators, routing info...)
// Telemetry - type 0, typically includes time tag in secondary header
// Telecommand - type 1, often has a command header including: Function code(what action to perform), checksum(verify command integrity)
//
// Time code - 8 bytes in this library(Following NASA), typically 6-10bytes
// Telecommand PUS header - Often 3 bytes (function code + checksum + spare)
//
// Payload is user data
//      For telemetry, could be instrument readings or spacecraft state data
//      For telecommands, this might be command arguments (turn on booster for 20 sec...)

// **Secondary header and payload should be seperate**
size_t encode_ccsds_packet(uint8_t *buf,
                           const ccsds_primary_header_t *h,
                           const ccsds_secondary_header_t *sh, size_t sec_len,
                           const uint8_t *payload, size_t payload_len)
{
    pack_ccsds_primary_header(buf, h);
    size_t offset = 6;

    // 2. Optional: secondary header
    if (h->sec_hdr_flag && sh && sec_len > 0)
    {
        // uint8_t raw_secondary[8];
        pack_ccsds_secondary_header(buf, sh);
        offset += sec_len;
    }

    // 3. Optional: payload
    if (payload && payload_len > 0)
    {
        memcpy(buf + offset, payload, payload_len);
        offset += payload_len;
    }

    return offset;
}

void unpack_ccsds_primary_header(const uint8_t *buf, ccsds_primary_header_t *h)
{
    h->version = (buf[0] >> 5) & 0x07;
    h->type = (buf[0] >> 4) & 0x01;
    h->sec_hdr_flag = (buf[0] >> 3) & 0x01;
    h->apid = ((buf[0] & 0x07) << 8) | buf[1];

    h->seq_flags = (buf[2] >> 6) & 0x03;
    h->seq_count = ((buf[2] & 0x3F) << 8) | buf[3];

    h->length = ((uint16_t)buf[4] << 8) | buf[5];
}

void unpack_ccsds_secondary_header(const uint8_t *buf, ccsds_secondary_header_t *h)
{
    h->coarse_time = ((uint32_t)buf[6] << 24) |
                     ((uint32_t)buf[7] << 16) |
                     ((uint32_t)buf[8] << 8) |
                     (uint32_t)buf[9];

    h->fine_time = ((uint32_t)buf[10] << 24) |
                   ((uint32_t)buf[11] << 16) |
                   ((uint32_t)buf[12] << 8) |
                   (uint32_t)buf[13];
}