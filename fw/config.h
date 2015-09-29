/* System configuration - see system/system.h */

#ifndef _CONFIG_H
#define _CONFIG_H 1

#include <avr/io.h>

#define IDN_STR "c4757p,MOD12003"

// Loop USART, port D, 115200
#define LOOP_USART  USARTD0
#define LOOP_PORT   PORTD
#define LOOP_REMAP
#define LOOP_BSEL   131
#define LOOP_BSCALE -3

// For bb_i2c_master
#define I2C_PORT_SCL    PORTD
#define I2C_VPORT_SCL   VPORT3
#define I2C_BIT_SCL     1
#define I2C_PORT_SDA    PORTD
#define I2C_VPORT_SDA   VPORT3
#define I2C_BIT_SDA     0
#define I2C_FREQ        400e3

// #include "usart_driver.h"

// extern USART_data_t usart_data;

/**********************************************************************
 * PINS */
/* Pin definitions */
/* Port, pin, name, direction, default value, drive type */
#define SYSTEM_CONF_PINS \
    X(A, 0, P_VREF,         D_IN,   0,  0)  /* AREF */                      \
    X(A, 1, P_REFGND,       D_IN,   0,  0)  /* ADC1 */                      \
    X(A, 2, P_ISET,         D_IN,   0,  0)  /* DAC0 */                      \
    X(A, 3, P_ISENSE,       D_IN,   0,  0)  /* ADC3 */                      \
    X(A, 4, P_VSENSE,       D_IN,   0,  0)  /* ADC4 */                      \
    X(A, 5, PA5,            D_IN,   0,  PORT_OPC_PULLDOWN_gc)               \
    X(A, 6, PA6,            D_IN,   0,  PORT_OPC_PULLDOWN_gc)               \
    X(A, 7, P_TEMPERATURE,  D_IN,   0,  0)  /* ADC7 */                      \
                                                                            \
    X(C, 0, PC0,            D_IN,   0,  PORT_OPC_PULLDOWN_gc)               \
    X(C, 1, P_OVP_PWM,      D_OUT,  0,  0)  /* OC4B */                      \
    X(C, 2, P_PREREG_SW,    D_OUT,  0,  0)  /* OC4C */                      \
    X(C, 3, P_SCR_TRIPPED,  D_IN,   0,  PORT_OPC_PULLUP_gc | PORT_INVEN_bm) \
    X(C, 4, P_ILIM,         D_IN,   0,  PORT_INVEN_bm)                      \
    X(C, 5, P_VLIM,         D_IN,   0,  PORT_INVEN_bm)                      \
    X(C, 6, P_POWER_EN,     D_OUT,  0,  PORT_INVEN_bm)                      \
    X(C, 7, PC7,            D_OUT,  0,  0)                                  \
                                                                            \
    X(D, 0, P_SDA,          D_IN,   0,  0)                                  \
    X(D, 1, P_SCL,          D_OUT,  1,  PORT_OPC_WIREDANDPULL_gc)           \
    X(D, 2, P_LED_CV,       D_OUT,  1,  0)                                  \
    X(D, 3, P_LED_CC,       D_OUT,  1,  0)                                  \
    X(D, 4, P_VPREREG,      D_IN,   0,  0)  /* ADC12 */                     \
    X(D, 5, P_BTN2,         D_IN,   0,  PORT_OPC_PULLUP_gc | PORT_INVEN_bm) \
    X(D, 6, P_RX,           D_IN,   0,  0)                                  \
    X(D, 7, P_TX,           D_OUT,  1,  0)                                  \
                                                                            \
    X(R, 0, P_BTN1,         D_IN,   0,  PORT_OPC_PULLUP_gc | PORT_INVEN_bm) \
    X(R, 1, PR1,            D_IN,   0,  PORT_OPC_PULLDOWN_gc)               \

/**********************************************************************
 * ADC channels */
#define ADC_REFGND  1
#define ADC_ISENSE  3
#define ADC_VSENSE  4
#define ADC_TEMP    7
#define ADC_PREREG  12

#endif /* _CONFIG_H */
