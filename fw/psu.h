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

/// Set the output voltage in mV, without enabling the supply.
void psu_vset(uint16_t mv);

/// Set the output current in mA, without enabling the supply.
void psu_iset(uint16_t ma);

/// Get the voltage setpoint in mV
uint16_t psu_get_vsetpt(void);

/// Get the current setpoint in mA
uint16_t psu_get_isetpt(void);

/// Set the preregulator voltage in mV (open-loop).
void psu_prereg_vset(uint16_t mv);

/**
 * Runs the fast cycle. This should be called every time a full ADC scan has
 * completed.
 */
void psu_fast_cycle(void);

/**
 * Runs the slow cycle. This should be called every slow system tick (about 250ms)
 */
void psu_slow_cycle(void);

#endif // _PSU_H
