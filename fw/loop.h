#ifndef _LOOP_H
#define _LOOP_H

#include "config.h"
#include "misc.h"
#include <inttypes.h>
#include <avr/pgmspace.h>

//  Interface code for MOD1 bus loop

// When a message is received bearing data, the data is buffered.
#define LOOP_DATA_BUF_SIZE  256

// Reserved addresses
#define LOOP_ADDR_RESPONSE  254
#define LOOP_ADDR_BROADCAST 255

#define LOOP_RXCINTLVL USART_RXCINTLVL_LO_gc
#define LOOP_DREINTLVL USART_DREINTLVL_MED_gc


#define CMD_NOP         0x00
#define CMD_IDN         0x01
#define CMD_ACK         0x70
#define CMD_NACK_BUFLEN 0x71
#define CMD_NACK_CRC    0x72
#define CMD_NACK_NO_CMD 0x73

/* Jump table, one per command */
extern void (* const CMD_HANDLERS[256])() PROGMEM;

/* Data from the received message. The handlers can read this. */
extern volatile uint8_t LOOP_ADDRESS;
extern volatile uint8_t LOOP_COMMAND;
extern volatile uint16_t LOOP_DATALEN;
extern volatile uint8_t LOOP_DATA_BUF[LOOP_DATA_BUF_SIZE];
extern volatile uint16_t LOOP_CRC;

/**
 * Send a command/message on the loop. Handlers can use this for responses.
 *
 * Warning: If calling from the main loop, MASK INTERRUPTS FIRST! This makes use of the
 * hardware CRC module. Do NOT use this from an interrupt level above LOOP_RXCINTLVL.
 *
 * @param addr - Destination address
 * @param cmd  - Command ID
 * @param data - Block of data to send. Can be NULL iff datalen == 0
 * @param datalen - Length of data to send
 */
void send_cmd(uint8_t addr, uint8_t cmd, const uint8_t *data, uint16_t datalen);

void send_cmd_P(uint8_t addr, uint8_t cmd, const uint8_t *data, uint16_t datalen);

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
