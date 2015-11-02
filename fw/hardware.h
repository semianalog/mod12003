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

void idac_init(void);
void idac_set(uint16_t value);

bool vdac_init(void);
bool vdac_set(uint16_t value);

void configure_adc(void);

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
UNUSED( static void crc_init(void) )
{
    CRC.CTRL = CRC_RESET_RESET1_gc;
    CRC.CTRL = CRC_SOURCE_IO_gc;
}

/**
 * Write one byte to the CRC module.
 */
UNUSED( static void crc_process_byte(uint8_t byte) )
{
    CRC.DATAIN = byte;
}

/**
 * Write a block to the CRC module.
 */
UNUSED( static void crc_process_bytes(const uint8_t* bytes, size_t n) )
{
    for (size_t i = 0; i < n; ++i) {
        crc_process_byte(bytes[i]);
    }
}

/**
 * Return the final checksum.
 */
UNUSED( static uint16_t crc_get_checksum(void) )
{
    CRC.STATUS |= CRC_BUSY_bm;
    while (CRC.STATUS & CRC_BUSY_bm);
    uint16_t checksum = ((uint16_t) CRC.CHECKSUM0) & 0xff;
    checksum |= ((uint16_t) CRC.CHECKSUM1 << 8) & 0xff00;
    return checksum;
}

/**
 * Check whether the checksum is zero.
 */
UNUSED( static bool crc_is_checksum_zero(void) )
{
    CRC.STATUS |= CRC_BUSY_bm;
    while (CRC.STATUS & CRC_BUSY_bm);
    return CRC.STATUS & CRC_ZERO_bm;
}


#endif // _HARDWARE_H
