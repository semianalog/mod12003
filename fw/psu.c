#include "psu.h"
#include "hardware.h"
#include "cal.h"
#include "timers.h"
#include "analog.h"

static int32_t          gs_cur_voltage_avg_numer    = 0;
static const int32_t    gs_cur_voltage_avg_denom    = INT32_C(32768);
static int32_t          gs_voltage_setpoint         = 0;
static uint16_t         gs_last_voltage             = 0;
static bool             gs_vset_updated             = false;
static int32_t          gs_correction_mv            = 0;

void psu_vset(uint16_t mv)
{
    gs_voltage_setpoint = mv;
    gs_vset_updated = true;
}

void psu_update(void)
{
    int32_t mv = gs_voltage_setpoint - gs_correction_mv;

    int64_t vslope = (uint64_t)(eeprom_read_dword(&CAL_C_VDACSLOPE_NUMER));
    int64_t voffset = (int16_t)(eeprom_read_word(&CAL_C_VDACOFFSET));

    int64_t word_i64 = (vslope * mv) / CAL_C_VDACSLOPE_DENOM + voffset;

    if (word_i64 < 0) {
        word_i64 = 0;
    } else if (word_i64 > UINT16_MAX) {
        word_i64 = UINT16_MAX;
    }

    uint16_t word = (uint16_t)(word_i64);
    vdac_set(word);
}

uint16_t psu_vget(void)
{
    uint16_t adc_word = gs_cur_voltage_avg_numer / gs_cur_voltage_avg_denom;
    int64_t slope = (uint64_t)(eeprom_read_dword(&CAL_C_VADCSLOPE_NUMER));
    int64_t voffset = (int16_t)(eeprom_read_word(&CAL_C_VADCOFFSET));

    int64_t mv_i64 = (slope * adc_word) / CAL_C_VADCSLOPE_DENOM + voffset;

    if (mv_i64 < 0) {
        mv_i64 = 0;
    } else if (mv_i64 > UINT16_MAX) {
        mv_i64 = UINT16_MAX;
    }

    return (uint16_t) mv_i64;
}

void psu_fast_cycle(void)
{
    // Collect and average ADC samples
    int32_t adc_word = (uint32_t)(get_adc_result(ADC_VSENSE)) * gs_cur_voltage_avg_denom;

    int32_t error = adc_word - gs_cur_voltage_avg_numer;

    gs_cur_voltage_avg_numer += error / ADC_LOWPASS_DIVISOR;

    uint16_t prereg = psu_prereg_vget();
    uint16_t postreg = gs_last_voltage;
    uint16_t prereg_margin = prereg - postreg;

    if (prereg < postreg || prereg_margin < REGULATOR_HEADROOM_LIMIT_MV) {
        psu_prereg_vset(gs_last_voltage + REGULATOR_HEADROOM_LIMIT_MV);
    }
}

void psu_slow_cycle(void)
{
    static int32_t s_voltage_error_accum = 0;

    if (gs_vset_updated) {
        // Clear everything out
        gs_vset_updated = false;
        s_voltage_error_accum = 0;
        psu_prereg_vset(gs_voltage_setpoint + REGULATOR_HEADROOM_LIMIT_MV);

    } else {

        gs_last_voltage = psu_vget();
        int32_t error = (int32_t)(gs_last_voltage) - gs_voltage_setpoint;

        s_voltage_error_accum += error;

        if (s_voltage_error_accum > VOLTAGE_WINDUP_LIMIT_RAW) {
            s_voltage_error_accum = VOLTAGE_WINDUP_LIMIT_RAW;
        } else if (s_voltage_error_accum < -VOLTAGE_WINDUP_LIMIT_RAW) {
            s_voltage_error_accum = -VOLTAGE_WINDUP_LIMIT_RAW;
        }

        gs_correction_mv = VOLTAGE_KI_NUMER * s_voltage_error_accum / VOLTAGE_KI_DENOM;

        psu_prereg_vset(gs_last_voltage + REGULATOR_HEADROOM_LIMIT_MV);

    }
    psu_update();
}

uint16_t psu_prereg_vget(void)
{
    uint32_t adc_word = get_adc_result(ADC_PREREG);

    // ADC_TOP + 1 is a tradeoff: a slight loss of accuracy for a
    // much faster divide by 0x8000
    return (adc_word * VREF_NOMINAL_MV * PREREG_DIV_DENOM) / ((ADC_TOP + 1) * PREREG_DIV_NUMER);
}

void psu_prereg_vset(uint16_t mv)
{
    // As before, PWM_TOP + 1 trades a bit of accuracy for efficiency.
    uint16_t dutycyc = (((uint32_t) PWM_TOP + 1) * mv) / INPUT_NOMINAL_MV;
    if (dutycyc > PWM_TOP) {
        dutycyc = PWM_TOP;
    }
    timer_pwmset_prereg(dutycyc);
}
