/* System configuration - see system/system.h */

#ifndef _CONFIG_H
#define _CONFIG_H 1

/******************************************************************************
 * USART */
#define AFW_USARTD0_BAUD        115200

/******************************************************************************
 * TWI */
#define AFW_TWIBB_BAUD  40000
#define AFW_TWIBB_SDA   P_SDA
#define AFW_TWIBB_SCL   P_SCL


/***
 * Non-AFW configuration */
#define LOOP_UART           AFW_UART_USARTD0
#define LOOP_UART_DREINT()  AFW_DEF_USART_DREINT(USARTD0)
#define LOOP_UART_RXCINT()  AFW_DEF_USART_RXCINT(USARTD0)

#define ADC_REFGND  1
#define ADC_ISENSE  3
#define ADC_VSENSE  4
#define ADC_TEMP    7
#define ADC_PREREG  12

#define IDN_STR "c4757p,MOD12003"

#endif /* _CONFIG_H */
