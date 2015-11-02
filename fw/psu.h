// Actual power supply control and regulation functions

#ifndef _PSU_H
#define _PSU_H

#include <inttypes.h>
#include <stdbool.h>

/**
 * Set the output voltage. Does not enable the regulator.
 *
 * @param mv output voltage, in millivolts
 * @return true on error
 */
bool psu_vset(uint16_t mv);

#endif // _PSU_H
