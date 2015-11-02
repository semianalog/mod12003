// This file holds any "magic" constants with physical units, including a lot of
// information about the analog section of the power supply.

#ifndef _ANALOG_H
#define _ANALOG_H

#include <inttypes.h>

/**
 * Integration constant for the voltage softloop.
 *
 * An additional factor of 1/TICK_PERIOD is implicit.
 */
static const int32_t    VOLTAGE_KI_NUMER            = 32;
static const int32_t    VOLTAGE_KI_DENOM            = 1024;

/**
 * How much to allow the voltage softloop integrator to wind up by.
 *
 * The softloop is only meant to make small corrections for things like op amp
 * offset voltage drift, so clamping the integrator prevents large spikes.
 */
static const uint16_t   VOLTAGE_WINDUP_LIMIT_MV     = 75;

/**
 * Voltage windup limit in integrator units, before application of the
 * integration constant. This is a computed constant.
 */
#define VOLTAGE_WINDUP_LIMIT_RAW (VOLTAGE_KI_DENOM * VOLTAGE_WINDUP_LIMIT_MV / VOLTAGE_KI_NUMER)

/**
 * Divisor for voltage ADC lowpass. This function is repeated every "fast" cycle:
 * lowpass_adc += (adc - lowpass_adc) / divisor;
 */
static const int32_t    ADC_LOWPASS_DIVISOR         = 16;


/**
 * Minimum overhead between preregulator and postregulator, in mV
 */
static const uint16_t   REGULATOR_HEADROOM_LIMIT_MV = 2500;

/**
 * Reference voltage, in mV
 */
static const uint16_t   VREF_NOMINAL_MV             = 2048;

/**
 * Preregulation sense divider ratio
 */
static const uint16_t   PREREG_DIV_NUMER            = 1;
static const uint16_t   PREREG_DIV_DENOM            = 12;

/**
 * Nominal system input voltage, in mV
 */
static const uint16_t   INPUT_NOMINAL_MV            = 24000;

#endif // _ANALOG_H
