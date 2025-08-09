#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ccsds_defs.h"
#include "ccsds_types.h"
#include "ccsds_header.h"
#include "ccsds_packing.h"

#define ASSERT_EQ(expected, actual, msg)                                    \
    if ((expected) != (actual))                                             \
    {                                                                       \
        printf("❌ %s (expected %d, got %d)\n", msg, (int)(expected), (int)(actual)); \
        return 1;                                                           \
    }                                                                       \
    else                                                                    \
    {                                                                       \
        printf("✅ %s\n", msg);                                             \
    }

#define ASSERT_MEMEQ(exp, act, n, msg)                                      \
    if (memcmp((exp), (act), (n)) != 0)                                      \
    {                                                                       \
        printf("❌ %s\n expected:", msg);                                    \
        for (size_t _i=0; _i<(n); ++_i) printf(" %02X", (unsigned)(exp)[_i]); \
        printf("\n got:     ");                                             \
        for (size_t _i=0; _i<(n); ++_i) printf(" %02X", (unsigned)(act)[_i]); \
        printf("\n");                                                       \
        return 1;                                                           \
    }                                                                       \
    else                                                                    \
    {                                                                       \
        printf("✅ %s\n", msg);                                             \
    }

/* Reusable builder for a valid primary header */
static inline ccsds_primary_header_t make_primary_ok(void)
{
    ccsds_primary_header_t h;
    ccsds_error_t ok = build_primary_header(&h,
        0, 1, 1, 100, 3, 16383, 65535
    );
    if (ok != CCSDS_OK) {
        printf("❌ make_primary_ok failed\n");
        exit(2);
    }
    return h;
}

#endif

