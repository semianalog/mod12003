#include "misc_math.h"
#include <stdbool.h>

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

void u16_to_str(char *s, uint16_t num)
{
    if (num == 0) {
        s[0] = '0';
        s[1] = 0;
    }

    char digits[5] = {0};
    uint16_t divisor = 10000u;

    for (uint8_t i = 0; i < 5; ++i) {
        digits[i] = '0';
        if (!num) {
            break;
        }
        while (num >= divisor) {
            num -= divisor;
            digits[i] += 1;
        }
        divisor /= 10;
    }

    bool saw_nonzero = false;
    for (uint8_t i = 0; i < 5; ++i) {
        if (digits[i] == '0' && !saw_nonzero) {
            continue;
        }
        saw_nonzero = true;
        *s = digits[i];
        ++s;
    }
    *s = 0;
}
