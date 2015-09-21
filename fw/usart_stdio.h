#ifndef _USART_STDIO_H
#define _USART_STDIO_H 1

#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include "usart_driver.h"

extern USART_data_t usart_stdio_data;

/**
 * Configure stdio to use the USART.
 *
 * You must also define interrupt handlers:
 *
 * ISR(USARTnn_RXC_vect) {
 *     USART_RXComplete (&usart_stdio_data);
 * }
 * ISR(USARTnn_DRE_vect) {
 *     USART_DataRegEmpty (&usart_stdio_data);
 * }
 */
void usart_stdio_init (USART_t *usart, uint16_t bsel, int8_t bscale);

#endif /* _USART_STDIO_H */
