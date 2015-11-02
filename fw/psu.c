#include "psu.h"
#include "hardware.h"
#include "cal.h"

bool psu_vset(uint16_t mv)
{
    int64_t vslope = (uint64_t)(eeprom_read_dword(&CAL_C_VDACSLOPE));
    int64_t voffset = (int16_t)(eeprom_read_word(&CAL_C_VDACOFFSET));

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

uint16_t psu_vget(void)
{
    uint16_t adc_word = get_adc_result(ADC_VSENSE);
    int64_t slope = (uint64_t)(eeprom_read_dword(&CAL_C_VADCSLOPE));
    int64_t voffset = (int16_t)(eeprom_read_word(&CAL_C_VADCOFFSET));

    int64_t mv_i64 = (slope * adc_word) / INT64_C(65536) + voffset;

    if (mv_i64 < 0) {
        mv_i64 = 0;
    } else if (mv_i64 > INT64_C(65535)) {
        mv_i64 = INT64_C(65535);
    }

    return (uint16_t) mv_i64;
}
