#ifndef _MAX5215_DAC_H
#define _MAX5215_DAC_H

#include <afw/twi.h>

#define MAXDAC_TWI AFW_TWIBB

#define MAXDAC_ADDR 0x38

#define MAXDAC_CODE_LOAD    0x01

#define MAXDAC_USER_CONFIG  0x08
#define MAXDAC_CLEAR_DEFAULT_bm 0x00
#define MAXDAC_CLEAR_ZERO_bm    0x10
#define MAXDAC_CLEAR_MID_bm     0x20
#define MAXDAC_CLEAR_FULL_bm    0x30
#define MAXDAC_AUX_DISABLE_bm   0x00
#define MAXDAC_AUX_nLDAC_bm     0x04
#define MAXDAC_AUX_nCLR_bm      0x08
#define MAXDAC_PWR_ON_bm        0x00
#define MAXDAC_PWR_HIZ_bm       0x01
#define MAXDAC_PWR_100k_bm      0x02
#define MAXDAC_PWR_1k_bm        0x03

/**
 * Return true on failure */
UNUSED( static bool maxdac_init(void) )
{
    MAXDAC_TWI.init();

    uint8_t buffer[3] = {MAXDAC_USER_CONFIG, 0x00,
        MAXDAC_CLEAR_ZERO_bm | MAXDAC_AUX_DISABLE_bm | MAXDAC_PWR_ON_bm };
    return MAXDAC_TWI.transact(MAXDAC_ADDR, buffer, 3, 0) == TWI_GOOD;
}

/**
 * Set DAC to a given word, 16-bit (even if DAC is 14-bit, assume 16 bits) */
UNUSED( static bool maxdac_set(uint16_t word) )
{
    uint8_t buffer[3] = {MAXDAC_CODE_LOAD, U16_HI(word), U16_LO(word)};
    return MAXDAC_TWI.transact(MAXDAC_ADDR, buffer, 3, 0) == TWI_GOOD;
}

#endif

