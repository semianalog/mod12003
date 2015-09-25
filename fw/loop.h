#ifndef _LOOP_H
#define _LOOP_H

#include "config.h"
#include "misc.h"
#include <inttypes.h>

//  Interface code for MOD1 bus loop

// When a message is received bearing data, the data is buffered.
#define LOOP_DATA_BUF_SIZE  256

// Reserved addresses
#define LOOP_ADDR_RESPONSE  254
#define LOOP_ADDR_BROADCAST 255

#define LOOP_RXCINTLVL USART_RXCINTLVL_LO_gc
#define LOOP_DREINTLVL USART_DREINTLVL_MED_gc


#define CMD_HI(c) (((c) & 0xff00) >> 8)
#define CMD_LO(c) ((c) & 0xff)

#define CMD_ACK         UINT16_C( 0x7000 )
#define CMD_NACK_BUFLEN UINT16_C( 0x7001 )
#define CMD_NACK_CRC    UINT16_C( 0x7002 )

UNUSED( static void configure_loop(void) )
{
    LOOP_USART.CTRLA = 0
        | USART_DRIE_bm
        | LOOP_RXCINTLVL
        | USART_TXCINTLVL_OFF_gc;

    LOOP_USART.CTRLC = 0
        | USART_CMODE_ASYNCHRONOUS_gc
        | USART_PMODE_DISABLED_gc
        | USART_CHSIZE_8BIT_gc;

    LOOP_USART.CTRLD = 0;

    LOOP_USART.BAUDCTRLA = UINT16_C(LOOP_BSEL) & 0xff;
    LOOP_USART.BAUDCTRLB = 0
        | ( (UINT16_C(LOOP_BSEL) & 0xf00) >> 8 )
        | ( ((uint8_t)(INT8_C(LOOP_BSCALE)) & 0x0f ) << 4 );

#   ifdef LOOP_REMAP
    LOOP_PORT.REMAP |= PORT_USART0_bm;
#   endif

    LOOP_USART.CTRLB = 0
        | USART_RXEN_bm
        | USART_TXEN_bm;
}

#endif // _LOOP_H
