/* The loop receiver uses the entirety of the LOW interrupt level, and events
 * triggered by the loop run in this level.
 */

#include "config.h"
#include "loop.h"
#include "hardware.h"
#include <avr/interrupt.h>

/**
 * Reset the CRC peripheral and process the last buffered message.
 */
static void crc_buffered_message(void)
{
    crc_init();
    crc_process_byte(g_loop_msg.addr);
    crc_process_byte(g_loop_msg.cmd);
    crc_process_bytes((uint8_t*) &g_loop_msg.datalen, 2);
    crc_process_bytes((uint8_t*) &g_loop_msg.data, g_loop_msg.datalen);
    crc_process_bytes((uint8_t*) &g_loop_msg.crc, 2);
}

enum recv_state {
    ADDRESS, COMMAND, DATALEN_1, DATALEN_0, DATA, CRC_1, CRC_0, FINISHED };

enum resp_mode {
    PASSTHROUGH,    /* This message not for me. Pass it on. */
    BROADCAST,      /* This message is for everyone. Handle it AND pass it on. */
    HANDLED,        /* This message is for me. Handle it. */
};

static enum recv_state  gs_state = ADDRESS;
static enum resp_mode   gs_mode  = PASSTHROUGH;
static uint8_t          gs_data  = 0;
static uint16_t         gs_data_idx = 0;


static enum recv_state handle_address(void)
{
    g_loop_msg.addr = gs_data;

    switch (gs_data) {
    case LOOP_ADDR_RESPONSE:
        gs_mode = PASSTHROUGH;
        break;
    case LOOP_ADDR_BROADCAST:
        gs_mode = BROADCAST;
        break;
    case 0:
        gs_mode = HANDLED;
        gs_data = LOOP_ADDR_RESPONSE;
        break;
    default:
        gs_mode = PASSTHROUGH;
        --gs_data;  // Transmit the address decremented
    }

    return COMMAND;
}


LOOP_UART_RXCINT()
{
    gs_data = LOOP_UART.get(false);

    switch(gs_state) {
    case ADDRESS:
        gs_state = handle_address();
        break;
    case COMMAND:
        // data = data;
        g_loop_msg.cmd = gs_data;
        gs_state = DATALEN_1;
        break;
    case DATALEN_1:
        // data = data;
        U16_BYTE(g_loop_msg.datalen, 0) = gs_data;
        gs_state = DATALEN_0;
        break;
    case DATALEN_0:
        // data = data;
        U16_BYTE(g_loop_msg.datalen, 1) = gs_data;
        gs_data_idx = 0;
        if (g_loop_msg.datalen) {
            gs_state = DATA;
        } else {
            gs_state = CRC_1;
        }
        break;
    case DATA:
        // data = data;
        g_loop_msg.data[gs_data_idx] = gs_data;
        ++gs_data_idx;

        if (gs_data_idx == g_loop_msg.datalen) {
            gs_state = CRC_1;
        }
        break;
    case CRC_1:
        // data = data;
        U16_BYTE(g_loop_msg.crc, 0) = gs_data;
        gs_state = CRC_0;
        break;
    case CRC_0:
        // data = data;
        U16_BYTE(g_loop_msg.crc, 1) = gs_data;
        gs_state = FINISHED;
        break;
    default:
        gs_state = ADDRESS;
    }

    switch (gs_mode) {
    case PASSTHROUGH:
        buffer_send(gs_data, false);
        break;
    case BROADCAST:
        if (gs_state == FINISHED) {
            // Same as HANDLED, but on correct receipt of the message, retransmit
            // it instead of sending an ACK.

            crc_buffered_message();
            if (crc_is_checksum_zero()) {
                send_msg(LOOP_ADDR_BROADCAST, g_loop_msg.cmd, (uint8_t *) &g_loop_msg.data, gs_data_idx);
            } else {
                uint16_t crc = crc_get_checksum();
                send_msg(LOOP_ADDR_RESPONSE, CMD_NACK_CRC, (uint8_t *) &crc, 2);
            }
        }
        break;
    case HANDLED:
        if (gs_state == FINISHED) {
            crc_buffered_message();
            if (crc_is_checksum_zero()) {
                void (*handler)() = pgm_read_ptr(&CMD_HANDLERS[g_loop_msg.cmd]);
                if (handler) {
                    handler();
                } else {
                    send_msg(LOOP_ADDR_RESPONSE, CMD_NACK_NO_CMD, NULL, 0);
                }
            } else {
                uint16_t crc = crc_get_checksum();
                send_msg(LOOP_ADDR_RESPONSE, CMD_NACK_CRC, (uint8_t *) &crc, 2);
            }
        }
        break;
    }
    if (gs_state == FINISHED) {
        gs_state = ADDRESS;
    }
}

