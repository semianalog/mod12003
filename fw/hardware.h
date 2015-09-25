#ifndef _HARDWARE_H
#define _HARDWARE_H 1

#include "config.h"
#include "misc.h"
#include <avr/io.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdbool.h>


// ADC positive mux bitmasks
#define ADC_CH(n)   ((n) << ADC_CH_MUXPOS_gp)

void configure_dac(void);

void set_iset_dac(uint16_t value);

void configure_adc(void);

uint16_t get_adc_result(uint8_t n);

/**
 * Initialize the CRC module to perform an operation. After initialization, you
 * can send data to it with crc_process_byte() and crc_process_bytes(). Note that
 * the CRC module is global and stateful: you cannot use it if it is being used
 * elsewhere!
 */
UNUSED( static void crc_init(void) )
{
    CRC.CTRL = CRC_RESET_RESET1_gc | CRC_SOURCE_IO_gc;
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
    while (n) {
        crc_process_byte(*bytes);
        ++bytes;
        --n;
    }
}

/**
 * Return the final checksum.
 */
UNUSED( static uint16_t crc_get_checksum(void) )
{
    CRC.STATUS |= CRC_BUSY_bm;
    while (!(CRC.STATUS & CRC_BUSY_bm));
    return CRC.CHECKSUM1 << 8 | CRC.CHECKSUM0;
}

/**
 * Check whether the checksum is zero.
 */
UNUSED( static bool crc_is_checksum_zero(void) )
{
    CRC.STATUS |= CRC_BUSY_bm;
    while (!(CRC.STATUS & CRC_BUSY_bm));
    return CRC.STATUS & CRC_ZERO_bm;
}

#endif // _HARDWARE_H
