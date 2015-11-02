// This file contains hardware support for both MCU hardware not supported
// by AFW, and external hardware.

#ifndef _HARDWARE_H
#define _HARDWARE_H

#include "config.h"
#include <afw/misc.h>
#include <avr/io.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdbool.h>

// ADC positive mux bitmasks
#define ADC_CH(n)   ((n) << ADC_CH_MUXPOS_gp)

// ADC maximum value
static const uint16_t ADC_TOP = 0x7fff;

void idac_init(void);
void idac_set(uint16_t value);

bool vdac_init(void);
bool vdac_set(uint16_t value);

void adc_init(void);

/**
 * Returns true if a reading was stored, or false if just waiting.
 */
bool adc_cycle(void);

/**
 * Cycle ADC until it has run a full scan, blocking until it has.
 */
void adc_scan(void);

/**
 * Report the number of ADC channels that are scanned.
 */
uint8_t adc_n_ch(void);

/**
 * Sample a given channel N times and return the sum. Performs full scans
 * until enough acquisitions have been made.
 */
uint32_t adc_sample_n(uint8_t channel, uint16_t times);

uint16_t get_adc_result(uint8_t n);

/**
 * Read from the production signatures row.
 *
 * @param idx - Index within the row. Get from offsetof(NVM_PROD_SIGNATURES_t, item)
 */
uint8_t read_prodsig(uint8_t idx);

/**
 * Read from the production signatures row. Computes offsetof automatically.
 *
 * @param item - Item to read, like WAFNUM
 */
#define READ_PRODSIG(item) read_prodsig(offsetof(NVM_PROD_SIGNATURES_t, item))

/**
 * Initialize the CRC module to perform an operation. After initialization, you
 * can send data to it with crc_process_byte() and crc_process_bytes(). Note that
 * the CRC module is global and stateful: you cannot use it if it is being used
 * elsewhere!
 */
void crc_init(void);

/**
 * Write one byte to the CRC module.
 */
void crc_process_byte(uint8_t byte);

/**
 * Write a block to the CRC module.
 */
void crc_process_bytes(const uint8_t *bytes, size_t n);

/**
 * Return the final checksum.
 */
uint16_t crc_get_checksum(void);

/**
 * Check whether the checksum is zero.
 */
bool crc_is_checksum_zero(void);


#endif // _HARDWARE_H
