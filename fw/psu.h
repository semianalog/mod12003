// Actual power supply control and regulation functions

#ifndef _PSU_H
#define _PSU_H

#include <inttypes.h>
#include <stdbool.h>

/**
 * Set the output voltage. Does not enable the regulator.
 *
 * @param mv output voltage, in millivolts
 */
void psu_vset(uint16_t mv);

/**
 * Get the output voltage.
 *
 * @return output voltage in mv
 */
uint16_t psu_vget(void);

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
 * Set the voltage of the buck preregulator, in mV, open-loop. Anything
 * greater than the measurable limit (24576 mV) is capped.
 */
void psu_prereg_vset(uint16_t mv);

#endif // _PSU_H
