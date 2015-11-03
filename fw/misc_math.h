#ifndef _MISC_MATH_H
#define _MISC_MATH_H

#include <inttypes.h>

static const int64_t    LINEAR_DENOM    = 65536;

/**
 * Linear function. Find y, where y = mx + b.
 *
 * Out-of-range results are clamped to 0 and UINT16_MAX.
 */
uint16_t linear(int32_t m_numer, uint16_t x, int16_t b) __attribute__((noinline));


#endif // _MISC_MATH_H
