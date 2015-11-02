#include "psu.h"
#include "hardware.h"
#include "cal.h"

bool psu_vset(uint16_t mv)
{
    // Converting mv to DAC word:
    // word = cal_slope * mv + cal_offset
    //
    // cal_slope = CAL_C_SLOPE / 65536
    // cal_offset = CAL_C_OFFSET

    int64_t vslope = (uint64_t)(eeprom_read_dword(&CAL_C_VSLOPE));
    int64_t voffset = (int16_t)(eeprom_read_word(&CAL_C_VOFFSET));

    int64_t word_i64 = (vslope * mv) / INT64_C(65536) + voffset;

    if (word_i64 < 0) {
        word_i64 = 0;
    } else if (word_i64 > INT64_C(65535)) {
        word_i64 = INT64_C(65535);
    }

    uint16_t word = (uint16_t)(word_i64);
    vdac_set(word);

    return false;
}
