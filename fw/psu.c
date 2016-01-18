#include <afw/pins.h>
#include <util/delay.h>
#include <util/atomic.h>
#include "psu.h"
#include "hardware.h"
#include "cal.h"
#include "timers.h"
#include "analog.h"
#include "misc_math.h"

static const int32_t    ADC_LOWPASS_DENOM           = INT32_C(32768);
static int32_t          gs_cur_voltage_avg_numer    = 0;
static int32_t          gs_cur_current_avg_numer    = 0;
static int16_t          gs_voltage_setpoint         = 0;
static int16_t          gs_current_setpoint         = 0;
static uint16_t         gs_last_voltage             = 0;

static int32_t          gs_correction_mv            = 0;
static volatile bool    gs_enabled                  = false;

void psu_enable(bool enabled)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        gs_enabled = enabled;
    }
}

bool psu_enabled(void)
{
    return PGET(P_LINREG_EN);
}

enum psu_reg_mode psu_get_reg_mode(void)
{
    bool cv = PGET(P_VLIM);
    bool cc = PGET(P_ILIM);

    if ((cc && cv) || (!cc && !cv)) {
        return PSU_OSCILLATING;
    } else if (cc) {
        return PSU_REG_CC;
    } else {
        return PSU_REG_CV;
    }
}

void psu_vset(uint16_t mv)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        gs_voltage_setpoint = mv;
    }
}

uint16_t psu_get_vsetpt(void)
{
    return ATOMIC_ACCESS(gs_voltage_setpoint);
}

uint16_t psu_get_isetpt(void)
{
    return ATOMIC_ACCESS(gs_current_setpoint);
}

/// Update the DACs
static void psu_update(void)
{
    int16_t mv = gs_voltage_setpoint - gs_correction_mv;
    uint16_t word = linear(CAL_DATA_VOLTAGE.dacslope, mv, CAL_DATA_VOLTAGE.dacoffset);
    vdac_set(word);
}

uint16_t psu_vget(void)
{
    uint32_t avg_numer = ATOMIC_ACCESS(gs_cur_voltage_avg_numer);
    uint16_t adc_word = avg_numer / ADC_LOWPASS_DENOM;
    return linear(CAL_DATA_VOLTAGE.adcslope, adc_word, CAL_DATA_VOLTAGE.adcoffset);
}

uint16_t psu_iget(void)
{
    uint32_t avg_numer = ATOMIC_ACCESS(gs_cur_current_avg_numer);
    uint16_t adc_word = avg_numer / ADC_LOWPASS_DENOM;
    return linear(CAL_DATA_CURRENT.adcslope, adc_word, CAL_DATA_CURRENT.adcoffset);
}

void psu_iset(uint16_t ma)
{
    uint16_t word = linear(CAL_DATA_CURRENT.dacslope, ma, CAL_DATA_CURRENT.dacoffset);
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        gs_current_setpoint = ma;
        idac_set(word); // XXX - get this out of a potential ISR call
    }
}

/**
 * Perform one step of an ADC lowpass filter.
 *
 * Constants used:
 *  - ADC_LOWPASS_DIVISOR:  filter function is   numer += (error / ADC_LOWPASS_DIVISOR)
 *  - ADC_LOWPASS_DENOM:    denominator for fixed-point storage
 *
 * @param channel - ADC channel to filter
 * @param numer - (in/out) numerator of filtered value
 */
static void lowpass_adc(uint8_t channel, int32_t *numer)
{
    int32_t adc_word = (uint32_t)(get_adc_result(channel)) * ADC_LOWPASS_DENOM;

    int32_t error = adc_word - *numer;

    *numer += error / ADC_LOWPASS_DIVISOR;
}

void psu_fast_cycle(void)
{
    if (!gs_enabled) {
        return;
    }

    lowpass_adc(ADC_VSENSE, &gs_cur_voltage_avg_numer);
    lowpass_adc(ADC_ISENSE, &gs_cur_current_avg_numer);

    uint16_t prereg = psu_prereg_vget();
    uint16_t postreg = gs_last_voltage;
    uint16_t prereg_margin = prereg - postreg;

    if (prereg < postreg || prereg_margin < REGULATOR_HEADROOM_LIMIT_MV) {
        psu_prereg_vset(gs_last_voltage + REGULATOR_HEADROOM_LIMIT_MV);
    } else {
        psu_prereg_vset(gs_voltage_setpoint + REGULATOR_HEADROOM_LIMIT_MV);
    }
}

static void update_leds(void)
{
    if (gs_enabled) {
        enum psu_reg_mode mode = psu_get_reg_mode();
        switch (mode) {
        case PSU_REG_CV:
            PSET(P_LEDCV);
            PCLR(P_LEDCC);
            break;
        case PSU_REG_CC:
            PSET(P_LEDCC);
            PCLR(P_LEDCV);
            break;
        case PSU_OSCILLATING:
        default:
            PSET(P_LEDCC);
            PSET(P_LEDCV);
        }
    } else {
        PCLR(P_LEDCC);
        PCLR(P_LEDCV);
        return;
    }
}

static void enable_regulator(void)
{
    gs_correction_mv = 0;
    vdac_set(0);

    // Force the regulator to saturate low during the edge of
    // LINREG_EN, because otherwise it tends to saturate high
    // and cause output blips due to integrator windup.
    PSET(P_LINREG_SAT);
    _delay_us(50);
    PSET(P_LINREG_EN);
    _delay_us(50);
    PCLR(P_LINREG_SAT);
}

static void disable_regulator(void)
{
    PCLR(P_LINREG_EN);
    psu_prereg_vset(0);
}

static void enable_or_disable(void)
{
    static bool s_last_enabled = false;

    if (gs_enabled && !s_last_enabled) {
        enable_regulator();
    } else if (!gs_enabled && s_last_enabled) {
        disable_regulator();
    }

    s_last_enabled = gs_enabled;
}

static void voltage_integrator_cycle(void)
{
    static int32_t s_voltage_error_accum = 0;

    gs_last_voltage = psu_vget();
    int16_t error = gs_last_voltage - gs_voltage_setpoint;

    // If the single-step error is greater than the windup limit, don't just clamp,
    // completely discard the sample. This happens at voltage setpoint steps.
    if (error > VOLTAGE_WINDUP_LIMIT_RAW || error < -VOLTAGE_WINDUP_LIMIT_RAW) {
        error = 0;
    }

    s_voltage_error_accum += error;

    if (s_voltage_error_accum > VOLTAGE_WINDUP_LIMIT_RAW) {
        s_voltage_error_accum = VOLTAGE_WINDUP_LIMIT_RAW;
    } else if (s_voltage_error_accum < -VOLTAGE_WINDUP_LIMIT_RAW) {
        s_voltage_error_accum = -VOLTAGE_WINDUP_LIMIT_RAW;
    }

    gs_correction_mv = VOLTAGE_KI_NUMER * s_voltage_error_accum / VOLTAGE_KI_DENOM;
}

void psu_slow_cycle(void)
{

    update_leds();
    enable_or_disable();

    if (gs_enabled) {
        voltage_integrator_cycle();
        psu_update();
    }
}

uint16_t psu_prereg_vget(void)
{
    uint32_t adc_word = ATOMIC_ACCESS(get_adc_result(ADC_PREREG));

    // ADC_TOP + 1 is a tradeoff: a slight loss of accuracy for a
    // much faster divide by 0x8000
    return (adc_word * VREF_NOMINAL_MV * PREREG_DIV_DENOM) / ((ADC_TOP + 1) * PREREG_DIV_NUMER);
}

uint16_t psu_temp_get(void)
{
    uint16_t adc_word = ATOMIC_ACCESS(get_adc_result(ADC_TEMP));

    uint16_t mv = adc_word / ((ADC_TOP + 1) / VREF_NOMINAL_MV);
    return linear(TEMP_SLOPE_NUMER, mv, TEMP_SLOPE_OFFSET);
}

uint16_t psu_powerdis_get_10mW(void)
{
    uint16_t prereg_mv = psu_prereg_vget();
    uint16_t out_ma = psu_iget();
    uint16_t out_mv = ATOMIC_ACCESS(gs_last_voltage);

    if (prereg_mv < out_mv)
    {
        // Output is being forced externally. Report maximum power, as there
        // is no other sensible answer here - we cannot know where the
        // current is flowing
        return 0xffffu;
    }

    uint16_t vdrop_mv = prereg_mv - out_mv;

    uint32_t power_uW = (uint32_t)(out_ma) * (uint32_t)(vdrop_mv);
    uint16_t power_10mW = power_uW / UINT32_C(10000);
    return power_10mW;
}

void psu_prereg_vset(uint16_t mv)
{
    // As before, PWM_TOP + 1 trades a bit of accuracy for efficiency.
    uint16_t dutycyc = (((uint32_t) PWM_TOP + 1) * mv) / INPUT_NOMINAL_MV;
    if (dutycyc > PWM_TOP) {
        dutycyc = PWM_TOP;
    }
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        timer_pwmset_prereg(dutycyc);
    }
}
