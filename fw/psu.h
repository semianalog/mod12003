// Actual power supply control and regulation functions

#ifndef _PSU_H
#define _PSU_H

#include <inttypes.h>
#include <stdbool.h>

/**
 * Enable or disable the power supply
 */
void psu_enable(bool enabled);

/**
 * Query power supply enabled status
 */
bool psu_enabled(void);

/**
 * PSU regulation modes
 */
enum psu_reg_mode {
    PSU_REG_CV,
    PSU_REG_CC,
    PSU_OSCILLATING,
};

/**
 * Query regulation mode
 */
enum psu_reg_mode psu_get_reg_mode(void);

/**
 * Set the output voltage. Does not enable the regulator.
 *
 * @param mv output voltage, in millivolts
 */
void psu_vset(uint16_t mv);

/**
 * Get the output voltage.
 *
 * @return output voltage in mV
 */
uint16_t psu_vget(void);

/**
 * Get the output current.
 *
 * @return output current in mA
 */
uint16_t psu_iget(void);

/**
 * Set the output current. Does not enable the regulator.
 *
 * @param ma output current, in milliamps
 */
void psu_iset(uint16_t ma);

/// Get the voltage setpoint in mV
uint16_t psu_get_vsetpt(void);

/// Get the current setpoint in mA
uint16_t psu_get_isetpt(void);

/**
 * Update the DACs.
 */
void psu_update(void);

/**
 * Runs the fast cycle. This should be called every time a full ADC scan has
 * completed.
 */
void psu_fast_cycle(void);

/**
 * Runs the slow cycle. This should be called every slow system tick (about 250ms)
 */
void psu_slow_cycle(void);

/**
 * Query the voltage of the buck preregulator, in mV.
 */
uint16_t psu_prereg_vget(void);

/**
 * Query the temperature of the regulator element, in 0.1degC.
 */
uint16_t psu_temp_get(void);

/// Query the power dissipation of the regulator element, in 10mW units.
uint16_t psu_powerdis_get_10mW(void);

/**
 * Set the voltage of the buck preregulator, in mV, open-loop. Anything
 * greater than the measurable limit (24576 mV) is capped.
 */
void psu_prereg_vset(uint16_t mv);

#endif // _PSU_H
