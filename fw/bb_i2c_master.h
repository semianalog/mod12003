#ifndef _BB_I2C_MASTER_H
#define _BB_I2C_MASTER_H 1

// Bit-banged I2C master for xmega
//
// To use, define the following in config.h (with examples):
//
// I2C_PORT_SCL     PORTD
// I2C_VPORT_SCL    VPORT3
// I2C_BIT_SCL      1
// I2C_PORT_SDA     PORTD
// I2C_VPORT_SDA    VPORT3
// I2C_BIT_SDA      0
// I2C_FREQ         400e3
//
// configure_i2c will not configure the vports. If you can't spare a vport for I2C,
// just leave those undefined, and real port access will be used instead.
// Beware that this is slower.

#include <inttypes.h>
#include <stdlib.h>

void configure_i2c(void);
void i2c_transfer(uint8_t addr, uint8_t *data, size_t tx_len, size_t rx_len);

#include "config.h"

#define CONCAT1(x,y,z) x ## y ## z
#define CONCAT2(x,y,z) CONCAT1(x,y,z)

#define I2C_PINCTRL_SCL CONCAT2(PIN, I2C_BIT_SCL, CTRL)
#define I2C_PINCTRL_SDA CONCAT2(PIN, I2C_BIT_SDA, CTRL)

#endif // _BB_I2C_MASTER_H
