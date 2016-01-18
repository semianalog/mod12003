// Measurement functions

#ifndef _MEASURE_H
#define _MEASURE_H

#include <inttypes.h>
#include <stdbool.h>

/// Perform one cycle of the voltage-sense and current-sense low pass filters.
void sense_filter_cycle(void);

/// Calculate and return the output voltage in mV.
uint16_t measured_voltage(void);

/// Return the previously measured voltage in mV, for speed.
uint16_t last_measured_voltage(void);

/// Calculate and return the output current in mA.
uint16_t measured_current(void);

/// Calculate and return the preregulator output voltage in mV.
uint16_t measured_prereg(void);

/// Calculate and return the temperature of the heatsink, in 0.1degC.
uint16_t measured_temperature(void);

/**
 * Calculate and return the regulator power dissipation, in 10mW units.
 * Uses last_measured_voltage, not measured_voltage!
 */
uint16_t measured_power_dissipation(void);

#endif // _MEASURE_H
