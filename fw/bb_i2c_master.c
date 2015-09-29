#include "bb_i2c_master.h"
#include <util/delay.h>
#include <stdbool.h>
#include <stdlib.h>

bool g_started = false;

static inline void SDA_SET(void) { I2C_VPORT_SDA.OUT |= (1 << I2C_BIT_SDA); }
static inline void SDA_CLR(void) { I2C_VPORT_SDA.OUT &= ~(1 << I2C_BIT_SDA); }
static inline bool SDA_GET(void) { return I2C_VPORT_SDA.IN & (1 << I2C_BIT_SDA); }
static inline void SCL_SET(void) { I2C_VPORT_SCL.OUT |= (1 << I2C_BIT_SCL); }
static inline void SCL_CLR(void) { I2C_VPORT_SCL.OUT &= ~(1 << I2C_BIT_SCL); }
static inline bool SCL_GET(void) { return I2C_VPORT_SCL.IN & (1 << I2C_BIT_SCL); }

static void i2c__delay(void)
{
    _delay_us(0.49/I2C_FREQ);
}

static void i2c__error(void)
{
    // XXX: do something
}

static void i2c__start(void)
{
    if (g_started) {
        SDA_SET();
        i2c__delay();
        // Clock stretching
        SCL_SET();
        while (!SCL_GET()); // XXX: add timeout
        i2c__delay();
    }
    if (!SDA_GET()) {
        i2c__error();
    }
    SDA_CLR();
    i2c__delay();
    SCL_CLR();
    g_started = true;
}

static void i2c__stop(void)
{
    SDA_CLR();
    i2c__delay();
    // Clock stretching
    SCL_SET();
    while (!SCL_GET()); // XXX: add timeout
    i2c__delay();
    if (!SDA_GET()) {
        i2c__error();
    }
    i2c__delay();
    g_started = false;
}

static void i2c__tx_bit(bool bit)
{
    if (bit) {
        SDA_SET();
    } else {
        SDA_CLR();
    }
    i2c__delay();
    // Clock stretching
    SCL_SET();
    while (!SCL_GET()); // XXX: add timeout

    SDA_SET();
    if (bit && !SDA_GET()) {
        // Something else is driving SDA
        i2c__error();
    }
    i2c__delay();
    SCL_CLR();
}

static bool i2c__rx_bit(void)
{
    bool bit;
    SDA_SET();
    i2c__delay();
    // Clock stretching
    SCL_SET();
    while (!SCL_GET()); // XXX: add timeout
    bit = SDA_GET();
    i2c__delay();
    SCL_CLR();
    return bit;
}

static bool i2c__tx_byte(bool send_start, bool send_stop, uint8_t byte)
{
    uint8_t bit;
    bool nack;
    if (send_start) {
        i2c__start();
    }
    for (bit = 0; bit < 8; ++bit) {
        i2c__tx_bit(byte & 0x80);
        byte <<= 1;
    }
    nack = i2c__rx_bit();
    if (send_stop) {
        i2c__stop();
    }
    return nack;
}

static uint8_t i2c__rx_byte(bool nack, bool send_stop) {
    uint8_t byte = 0;
    uint8_t bit;
    for (bit = 0; bit < 8; ++bit) {
        byte = (byte << 1) | i2c__rx_bit();
    }
    i2c__tx_bit(nack);
    if (send_stop) {
        i2c__stop();
    }
    return byte;
}

void configure_i2c(void)
{
    I2C_PORT_SCL.DIRCLR |= (1 << I2C_BIT_SCL);
    I2C_PORT_SDA.DIRCLR |= (1 << I2C_BIT_SDA);

    I2C_PORT_SCL.I2C_PINCTRL_SCL = PORT_OPC_WIREDANDPULL_gc;
    I2C_PORT_SDA.I2C_PINCTRL_SDA = PORT_OPC_WIREDANDPULL_gc;
}

void i2c_transfer(uint8_t addr, uint8_t *data, size_t tx_len, size_t rx_len)
{
    size_t i;
    i2c__tx_byte(true, false, addr);
    for (i = 0; i < tx_len; ++i) {
        i2c__tx_byte(false, i == (tx_len - 1) && !rx_len, data[i]);
    }

    if (rx_len) {
        i2c__tx_byte(true, false, addr | 1);
        for (i = 0; i < rx_len; ++i) {
            data[i] = i2c__rx_byte(false, i == (rx_len - 1));
        }
    }
}

