#include <afw/pins.h>
#include <util/delay.h>
#include "psu.h"
#include "hardware.h"
#include "cal.h"
#include "timers.h"
#include "analog.h"
#include "misc_math.h"

static const int32_t    ADC_LOWPASS_DENOM           = INT32_C(32768);
static int32_t          gs_cur_voltage_avg_numer    = 0;
static int32_t          gs_cur_current_avg_numer    = 0;
static int32_t          gs_voltage_setpoint         = 0;
static uint16_t         gs_last_voltage             = 0;
static bool             gs_vset_updated             = false;
static int32_t          gs_correction_mv            = 0;
static volatile bool    gs_enabled                  = false;

void psu_enable(bool enabled)
{
    gs_enabled = enabled;
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
    gs_voltage_setpoint = mv;
    gs_vset_updated = true;
}

void psu_update(void)
{
    int32_t mv = gs_voltage_setpoint - gs_correction_mv;
    uint16_t word = linear(CAL_DATA_VOLTAGE.dacslope, mv, CAL_DATA_VOLTAGE.dacoffset);
    vdac_set(word);
}

uint16_t psu_vget(void)
{
    uint16_t adc_word = gs_cur_voltage_avg_numer / ADC_LOWPASS_DENOM;
    return linear(CAL_DATA_VOLTAGE.adcslope, adc_word, CAL_DATA_VOLTAGE.adcoffset);
}

uint16_t psu_iget(void)
{
    uint16_t adc_word = gs_cur_current_avg_numer / ADC_LOWPASS_DENOM;
    return linear(CAL_DATA_CURRENT.adcslope, adc_word, CAL_DATA_CURRENT.adcoffset);
}

void psu_iset(uint16_t ma)
{
    uint16_t word = linear(CAL_DATA_CURRENT.dacslope, ma, CAL_DATA_CURRENT.dacoffset);
    idac_set(word);
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
    lowpass_adc(ADC_VSENSE, &gs_cur_voltage_avg_numer);
    lowpass_adc(ADC_ISENSE, &gs_cur_current_avg_numer);

    uint16_t prereg = psu_prereg_vget();
    uint16_t postreg = gs_last_voltage;
    uint16_t prereg_margin = prereg - postreg;

    if (prereg < postreg || prereg_margin < REGULATOR_HEADROOM_LIMIT_MV) {
        psu_prereg_vset(gs_last_voltage + REGULATOR_HEADROOM_LIMIT_MV);
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
        PCLR(P_LINREG_EN);
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

void psu_slow_cycle(void)
{
    static int32_t s_voltage_error_accum = 0;
    static bool s_last_enabled = false;

    update_leds();

    if (gs_enabled && !s_last_enabled) {
        enable_regulator();
    }

    s_last_enabled = gs_enabled;

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
