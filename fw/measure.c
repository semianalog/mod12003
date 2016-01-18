#include "measure.h"
#include "hardware.h"
#include "misc_math.h"
#include "analog.h"
#include "cal.h"

static const int32_t    ADC_LOWPASS_DENOM           = INT32_C(32768);
static int32_t          gs_cur_voltage_avg_numer    = 0;
static int32_t          gs_cur_current_avg_numer    = 0;
static uint16_t         gs_last_voltage             = 0;

/**
 * Perform one step of an ADC lowpass filter.
 *
 * Constants used:
 *  - ADC_LOWPASS_DIVISOR:  filter function is   numer += (error / ADC_LOWPASS_DIVISOR)
 *  - ADC_LOWPASS_DENOM:    denominator for fixed-point storage
 *
 * @param channel - ADC channel to filter
 * @param numer - (in/out) numerator of filtered value
 * @return - the new numerator
 */
static int32_t lowpass_adc(uint8_t channel, int32_t numer)
{
    int32_t adc_word = (uint32_t)(get_adc_result(channel)) * ADC_LOWPASS_DENOM;

    int32_t error = adc_word - numer;

    return numer + error / ADC_LOWPASS_DIVISOR;
}

void sense_filter_cycle(void)
{
    gs_cur_voltage_avg_numer = lowpass_adc(ADC_VSENSE, gs_cur_voltage_avg_numer);
    gs_cur_current_avg_numer = lowpass_adc(ADC_ISENSE, gs_cur_current_avg_numer);
}

uint16_t measured_voltage(void)
{
    uint32_t avg_numer = ATOMIC_ACCESS(gs_cur_voltage_avg_numer);
    uint16_t adc_word = avg_numer / ADC_LOWPASS_DENOM;
    uint16_t mv = linear(CAL_DATA_VOLTAGE.adcslope, adc_word, CAL_DATA_VOLTAGE.adcoffset);

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        gs_last_voltage = mv;
    }

    return mv;
}

uint16_t last_measured_voltage(void)
{
    uint16_t mv;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        mv = gs_last_voltage;
    }
    return mv;
}

uint16_t measured_current(void)
{
    uint32_t avg_numer = ATOMIC_ACCESS(gs_cur_current_avg_numer);
    uint16_t adc_word = avg_numer / ADC_LOWPASS_DENOM;
    return linear(CAL_DATA_CURRENT.adcslope, adc_word, CAL_DATA_CURRENT.adcoffset);
}

uint16_t measured_prereg(void)
{
    uint32_t adc_word = ATOMIC_ACCESS(get_adc_result(ADC_PREREG));

    // ADC_TOP + 1 is a tradeoff: a slight loss of accuracy for a
    // much faster divide by 0x8000
    return (adc_word * VREF_NOMINAL_MV * PREREG_DIV_DENOM) / ((ADC_TOP + 1) * PREREG_DIV_NUMER);
}

uint16_t measured_temperature(void)
{
    uint16_t adc_word = ATOMIC_ACCESS(get_adc_result(ADC_TEMP));

    uint16_t mv = adc_word / ((ADC_TOP + 1) / VREF_NOMINAL_MV);
    return linear(TEMP_SLOPE_NUMER, mv, TEMP_SLOPE_OFFSET);
}

uint16_t measured_power_dissipation(void)
{
    uint16_t prereg_mv = measured_prereg();
    uint16_t out_ma = measured_current();
    uint16_t out_mv = last_measured_voltage();

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

