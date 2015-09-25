#include "config.h"
#include "loop.h"
#include "hardware.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>

#define LOOP_USART_RXC_vect LOOP_USART ## _RXC_vect
#define LOOP_USART_DRE_vect LOOP_USART ## _DRE_vect

// Length of 256 is critical, as for efficiency we are depending on
// uint8_t overflow.
static volatile uint8_t LOOP_BUFFER[256];
static volatile uint8_t LOOP_BUFFER_HEAD = 0;
static volatile uint8_t LOOP_BUFFER_TAIL = 0;

// This is the buffer for received data. It does not have to be a fixed
// size; the size is defined in loop.h
volatile uint8_t LOOP_DATA_BUF[LOOP_DATA_BUF_SIZE];

volatile uint16_t LOOP_COMMAND;
volatile uint8_t *LOOP_COMMAND_BYTES = (volatile uint8_t *) &LOOP_COMMAND;
volatile uint16_t LOOP_DATALEN;
volatile uint8_t *LOOP_DATALEN_BYTES = (volatile uint8_t *) &LOOP_DATALEN;
volatile uint16_t LOOP_CRC;
volatile uint8_t *LOOP_CRC_BYTES = (volatile uint8_t *) &LOOP_CRC;

static bool buffer_push(uint8_t byte);
static uint8_t buffer_len(void);
static uint8_t buffer_pop(void);
static void enable_dre(void);
static void disable_dre(void);
static void buffer_send(uint8_t byte, bool spin, bool crc);
static void buffer_send_bytes(const uint8_t *data, size_t len, bool spin, bool crc);

static void send_cmd(uint8_t addr, uint16_t cmd, const uint8_t *data, uint16_t datalen);

static void crc_buffered_message(void);

// Returns true on failure
// Does NOT disable interrupts!
static bool buffer_push(uint8_t byte)
{
    if (LOOP_BUFFER_TAIL == LOOP_BUFFER_HEAD - 1) {
        return true;
    }

    LOOP_BUFFER[LOOP_BUFFER_TAIL] = byte;
    ++LOOP_BUFFER_TAIL;
    return false;
}

static uint8_t buffer_len(void)
{
    return LOOP_BUFFER_TAIL - LOOP_BUFFER_HEAD;
}

// No error-reporting mechanism, undefined behavior if no byte is available!
static uint8_t buffer_pop(void)
{
    uint8_t byte = LOOP_BUFFER[LOOP_BUFFER_HEAD];
    ++LOOP_BUFFER_HEAD;
    return byte;
}

static void enable_dre(void)
{
    uint8_t temp = LOOP_USART.CTRLA;
    temp = (temp & ~USART_DREINTLVL_gm) | LOOP_DREINTLVL;
    LOOP_USART.CTRLA = temp;
}

static void disable_dre(void)
{
    uint8_t temp = LOOP_USART.CTRLA;
    temp = (temp & ~USART_DREINTLVL_gm) | USART_DREINTLVL_OFF_gc;
    LOOP_USART.CTRLA = temp;
}

static void buffer_send(uint8_t byte, bool spin, bool crc)
{
    do {
        disable_dre();
        if (!buffer_push(byte)) {
            enable_dre();
            break;
        }
    } while (spin);
    if (crc)
        crc_process_byte(byte);
}

static void buffer_send_bytes(const uint8_t *data, size_t len, bool spin, bool crc)
{
    for (size_t i = 0; i < len; ++i) {
        buffer_send(data[i], spin, crc);
    }
}

/**
 * Warning: uses CRC! */
static void send_cmd(uint8_t addr, uint16_t cmd, const uint8_t *data, uint16_t datalen)
{
    crc_init();
    buffer_send(addr, true, true);
    buffer_send(CMD_LO(cmd), true, true);
    buffer_send(CMD_HI(cmd), true, true);
    buffer_send(CMD_LO(datalen), true, true);
    buffer_send(CMD_HI(datalen), true, true);
    buffer_send_bytes(data, datalen, true, true);
    uint16_t crc = crc_get_checksum();
    buffer_send(crc & 0xff, true, false); // CRC
    buffer_send((crc & 0xff00) >> 8, true, false); // CRC
}

static void crc_buffered_message(void)
{
    crc_init();
    crc_process_byte(LOOP_ADDR_BROADCAST);
    crc_process_bytes((uint8_t*) LOOP_COMMAND_BYTES, 2);
    crc_process_bytes((uint8_t*) LOOP_DATALEN_BYTES, 2);
    crc_process_bytes((uint8_t*) LOOP_DATA_BUF, LOOP_DATALEN);
    crc_process_bytes((uint8_t*) LOOP_CRC_BYTES, 2);
}

ISR(USARTD0_RXC_vect)
{
    // State machine to handle messages
    static uint16_t data_buf_idx = 0;

    enum recv_state {
        ADDRESS, COMMAND_1, COMMAND_0, DATALEN_1, DATALEN_0, DATA, CRC_1, CRC_0, FINISHED };
    static enum recv_state state = ADDRESS;

    enum resp_mode {
        PASSTHROUGH, BROADCAST, HANDLED };
    static enum resp_mode mode = PASSTHROUGH;

    uint8_t data = LOOP_USART.DATA;

    switch(state) {
    case ADDRESS:
        if (data == LOOP_ADDR_RESPONSE) {
            mode = PASSTHROUGH;
        } else if (data == LOOP_ADDR_BROADCAST) {
            mode = BROADCAST;
        } else if (data > 0) {
            mode = PASSTHROUGH;
            --data;
        } else {
            mode = HANDLED;
            data = LOOP_ADDR_RESPONSE;
        }
        state = COMMAND_1;
        break;
    case COMMAND_1:
        // data = data;
        LOOP_COMMAND_BYTES[0] = data;
        state = COMMAND_0;
        break;
    case COMMAND_0:
        // data = data;
        LOOP_COMMAND_BYTES[1] = data;
        state = DATALEN_1;
        break;
    case DATALEN_1:
        // data = data;
        LOOP_DATALEN_BYTES[0] = data;
        state = DATALEN_0;
        break;
    case DATALEN_0:
        // data = data;
        LOOP_DATALEN_BYTES[1] = data;
        data_buf_idx = 0;
        if (LOOP_DATALEN) {
            state = DATA;
        } else {
            state = CRC_1;
        }
        break;
    case DATA:
        // data = data;
        LOOP_DATA_BUF[data_buf_idx] = data;
        ++data_buf_idx;

        if (data_buf_idx == LOOP_DATALEN) {
            state = CRC_1;
        }
        break;
    case CRC_1:
        // data = data;
        LOOP_CRC_BYTES[0] = data;
        state = CRC_0;
        break;
    case CRC_0:
        // data = data;
        LOOP_CRC_BYTES[1] = data;
        state = FINISHED;
        break;
    default:
        state = ADDRESS;
    }

    switch (mode) {
    case PASSTHROUGH:
        buffer_send(data, /* spin */ true, /* crc */ false);
        break;
    case BROADCAST:
        if (state == FINISHED) {
            // Same as HANDLED, but on correct receipt of the message, retransmit
            // it instead of sending an ACK.

            crc_buffered_message();
            if (crc_is_checksum_zero()) {
                send_cmd(LOOP_ADDR_BROADCAST, LOOP_COMMAND, (uint8_t *) LOOP_DATA_BUF, data_buf_idx);
            } else {
                uint16_t crc = crc_get_checksum();
                send_cmd(LOOP_ADDR_RESPONSE, CMD_NACK_CRC, (uint8_t *) &crc, 2);
            }
        }
        break;
    case HANDLED:
        if (state == FINISHED) {
            crc_buffered_message();
            if (crc_is_checksum_zero()) {
                send_cmd(LOOP_ADDR_RESPONSE, CMD_ACK, NULL, 0);
            } else {
                uint16_t crc = crc_get_checksum();
                send_cmd(LOOP_ADDR_RESPONSE, CMD_NACK_CRC, (uint8_t *) &crc, 2);
            }
        }
        break;
    }
    if (state == FINISHED) {
        state = ADDRESS;
    }
}

ISR(USARTD0_DRE_vect)
{
    if (!buffer_len()) {
        disable_dre();
    } else {
        LOOP_USART.DATA = buffer_pop();
    }
}
