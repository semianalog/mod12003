#ifndef _MISC_MATH_H
#define _MISC_MATH_H

#include <inttypes.h>

#define NOINLINE    __attribute__((noinline))
#define PURE        __attribute__((pure))

static const int64_t    LINEAR_DENOM    = 65536;

/**
 * Linear function. Find y, where y = mx + b.
 *
 * Out-of-range results are clamped to 0 and UINT16_MAX.
 */
uint16_t linear(int32_t m_numer, uint16_t x, int16_t b) NOINLINE PURE;

/**
 * Lightweight u16-to-string routine.
 *
 * @param str - string buffer, must have at least six bytes
 * @param num - number to convert
 */
void u16_to_str(char *s, uint16_t num);

#endif // _MISC_MATH_H
