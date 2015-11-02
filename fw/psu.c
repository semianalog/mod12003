#include "psu.h"
#include "hardware.h"
#include "cal.h"
#include "timers.h"

static int32_t gs_current_voltage_averaged = 0;
static int32_t gs_voltage_setpoint = 0;
static int32_t gs_voltage_error = 0;
static int32_t gs_voltage_error_accum = 0;
static uint16_t gs_last_voltage = 0;
static bool gs_vset_updated = false;
static int8_t gs_correction_railed = 0;

void psu_vset(uint16_t mv)
{
    gs_voltage_setpoint = mv;
    gs_vset_updated = true;
}

void psu_update(void)
{
    const int32_t KI = 32;
    int32_t correction = KI * gs_voltage_error_accum / 1024;

    // Do not correct by much. This is only to offset thermal error and such, we
    // don't want windup issues etc
    if (correction > 75) {
        correction = 75;
        gs_correction_railed = 1;
    } else if (correction < -75) {
        correction = -75;
        gs_correction_railed = -1;
    } else {
        gs_correction_railed = 0;
    }

    int32_t mv = gs_voltage_setpoint - correction;

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
}

uint16_t psu_vget(void)
{
    uint16_t adc_word = gs_current_voltage_averaged >> 15;
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

void psu_fast_cycle(void)
{
    // Collect and average ADC samples
    int32_t adc_word = (uint32_t)(get_adc_result(ADC_VSENSE)) << 15;

    int32_t error = adc_word - gs_current_voltage_averaged;

    gs_current_voltage_averaged += error / 16;

    uint16_t prereg = psu_prereg_vget();
    uint16_t postreg = gs_last_voltage;
    uint16_t prereg_margin = prereg - postreg;

    if (prereg < postreg || prereg_margin < 2500) {
        psu_prereg_vset(gs_last_voltage + 2500);
    }
}

void psu_slow_cycle(void)
{
    if (gs_vset_updated) {
        gs_vset_updated = false;
        gs_voltage_error = 0;
        gs_voltage_error_accum = 0;
        psu_prereg_vset(gs_voltage_setpoint + 2500);
    } else {
        gs_last_voltage = psu_vget();
        gs_voltage_error = (int32_t)(gs_last_voltage) - gs_voltage_setpoint;

        // If the integrator is railed, only move it towards center
        if (gs_correction_railed > 0 && gs_voltage_error > 0) {
            gs_voltage_error = 0;
        } else if (gs_correction_railed < 0 && gs_voltage_error < 0) {
            gs_voltage_error = 0;
        }

        gs_voltage_error_accum += gs_voltage_error;
        psu_prereg_vset(gs_last_voltage + 2500);
    }
    psu_update();
}

uint16_t psu_prereg_vget(void)
{
    uint32_t adc_word = get_adc_result(ADC_PREREG);

    return (adc_word * 2048 * 12) / 32768;
}

void psu_prereg_vset(uint16_t mv)
{
    uint16_t dutycyc = mv / 8 / 12;
    if (dutycyc > 255) {
        dutycyc = 255;
    }
    timer_pwmset_prereg(dutycyc);
}
