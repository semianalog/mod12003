#ifndef _HARDWARE_H
#define _HARDWARE_H 1

#include "config.h"
#include <avr/io.h>
#include <inttypes.h>


// ADC positive mux bitmasks
#define ADC_CH(n)   ((n) << ADC_CH_MUXPOS_gp)

void configure_dac(void);

void set_iset_dac(uint16_t value);

void configure_adc(void);

uint16_t get_adc_result(uint8_t n);

#endif // _HARDWARE_H
