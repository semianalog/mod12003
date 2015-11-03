#include "misc_math.h"

uint16_t linear(int32_t m_numer, uint16_t x, int16_t b)
{
    int64_t m_numer_64 = m_numer;
    int64_t x_64 = (int64_t)(uint64_t)(x);  // ensure zero-extend cast

    int32_t mx = (m_numer_64 * x_64) / LINEAR_DENOM;

    int32_t res_32 = mx + b;

    if (res_32 > UINT16_MAX) {
        return UINT16_MAX;
    } else if (res_32 < 0) {
        return 0;
    } else {
        return res_32;
    }
}
