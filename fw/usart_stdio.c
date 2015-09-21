#include "usart_driver.h"
#include <stdio.h>
#include <avr/io.h>

USART_data_t usart_stdio_data;

int usart_putchar (char c, FILE *stream)
{
    if (c == '\n') {
        usart_putchar ('\r', stream);
    }

    while (!USART_TXBuffer_FreeSpace (&usart_stdio_data));
    USART_TXBuffer_PutByte (&usart_stdio_data, c);
    return 0;
}

int usart_getchar (FILE *stream)
{
    (void) stream;
    if (USART_RXBufferData_Available (&usart_stdio_data)) {
        return USART_RXBuffer_GetByte (&usart_stdio_data);
    } else {
        return -1;
    }
}

FILE usart_stdout = FDEV_SETUP_STREAM (usart_putchar, NULL, _FDEV_SETUP_WRITE);
FILE usart_stdin = FDEV_SETUP_STREAM (NULL, usart_getchar, _FDEV_SETUP_READ);

void
usart_stdio_init (USART_t *usart, uint16_t bsel, int8_t bscale)
{
    USART_InterruptDriver_Initialize (&usart_stdio_data, usart,
            USART_DREINTLVL_LO_gc);

    USART_Format_Set (usart_stdio_data.usart, USART_CHSIZE_8BIT_gc,
            USART_PMODE_DISABLED_gc, false);

    USART_RxdInterruptLevel_Set (usart_stdio_data.usart, USART_RXCINTLVL_LO_gc);

    USART_Baudrate_Set (usart, bsel, bscale);

    USART_Rx_Enable (usart_stdio_data.usart);
    USART_Tx_Enable (usart_stdio_data.usart);

    PMIC.CTRL |= PMIC_LOLVLEN_bm;

    stdout = &usart_stdout;
    stdin = &usart_stdin;
}
