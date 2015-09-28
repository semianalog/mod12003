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
//
// Note that this scheme means the buffer can only store _255_ bytes, and a
// 256th unused byte will rotate through the buffer space.
//
// 'tail' stores the position of the _next_ byte to be pushed after the last
// byte, not the last byte itself. This permits (head == tail) to signify
// "buffer empty".
static volatile struct {
    uint8_t data[256];
    uint8_t head;
    uint8_t tail;
} gs_buffer = {{0}, 0, 0};

volatile struct loop_msg g_loop_msg;

/**
 * Push a byte to the transmit buffer. Return immediately if the buffer was full.
 *
 * Warning: does NOT disable or enable interrupts. Will not trigger a
 * transmission, and can cause problems if used when the interrupts are
 * active.
 *
 * @param byte - the byte to push onto the buffer.
 * @return true iff the buffer was full
 */
static bool buffer_push(uint8_t byte)
{
    if (gs_buffer.tail == gs_buffer.head - 1) {
        return true;
    }

    gs_buffer.data[gs_buffer.tail] = byte;
    ++gs_buffer.tail;
    return false;
}

/**
 * Return the length of data in the buffer.
 */
static uint8_t buffer_len(void)
{
    return gs_buffer.tail - gs_buffer.head;
}

/**
 * Pop a byte off the buffer and return it. You _must_ check that there is a byte
 * available; nasal demons will result if not (the buffer will suddenly contain
 * 255 bytes of garbage/old data).
 */
static uint8_t buffer_pop(void)
{
    uint8_t byte = gs_buffer.data[gs_buffer.head];
    ++gs_buffer.head;
    return byte;
}

/**
 * Enable the Data Register Empty interrupt at the interrupt level specified in loop.h
 */
static void enable_dre(void)
{
    LOOP_USART.CTRLA = (LOOP_USART.CTRLA & ~USART_DREINTLVL_gm) | LOOP_DREINTLVL;
}

/**
 * Disable the Data Register Empty interrupt.
 */
static void disable_dre(void)
{
    LOOP_USART.CTRLA = (LOOP_USART.CTRLA & ~USART_DREINTLVL_gm) | USART_DREINTLVL_OFF_gc;
}

/**
 * Push a byte to the buffer and trigger a transmission. This does clear and set the
 * interrupts, and must not run from the same interrupt level as LOOP_DREINTLVL.
 *
 * This function will spin until the buffer is ready, so do not call it if the
 * USART is not enabled or the DRE interrupt will not run for any reason. It will
 * result in a deadlock in that case.
 *
 * @param byte - the byte to send
 * @param crc - iff true, also push the byte to the CRC peripheral
 */
void buffer_send(uint8_t byte, bool crc)
{
    for (;;) {
        disable_dre();
        if (!buffer_push(byte)) {
            enable_dre();
            break;
        }
    }
    if (crc)
        crc_process_byte(byte);
}

/**
 * Transmit multiple bytes. This does clear and set the interrupts, and must not be
 * run from the same interrupt level as LOOP_DREINTLVL.
 *
 * This function will spin until the buffer is ready, so do not call it if the
 * USART is not enabled or the DRE interrupt will not run for any reason. It will
 * result in a deadlock in that case.
 *
 * @param data - the data to send
 * @param len - length of the data
 * @param crc - iff true, also push the data to the CRC peripheral
 */
void buffer_send_bytes(const uint8_t *data, size_t len, bool crc)
{
    for (size_t i = 0; i < len; ++i) {
        buffer_send(data[i], crc);
    }
}

/**
 * Send the header of the message - everything before the data block.
 */
static void send_msg_header(uint8_t addr, uint8_t cmd, uint16_t datalen)
{
    crc_init();
    buffer_send(addr, true);
    buffer_send(cmd, true);
    buffer_send(U16_LO(datalen), true);
    buffer_send(U16_HI(datalen), true);
}

/**
 * Get the checksum from the CRC peripheral and send it.
 */
static void send_checksum(void) {
    uint16_t crc = crc_get_checksum();
    buffer_send(U16_HI(crc), false);
    buffer_send(U16_LO(crc), false);
}

void send_msg(uint8_t addr, uint8_t cmd, const uint8_t *data, uint16_t datalen)
{
    send_msg_header(addr, cmd, datalen);
    buffer_send_bytes(data, datalen, true);
    send_checksum();
}

void send_msg_P(uint8_t addr, uint8_t cmd, const uint8_t *data, uint16_t datalen)
{
    send_msg_header(addr, cmd, datalen);
    for (size_t i = 0; i < datalen; ++i) {
        buffer_send(pgm_read_byte(&data[i]), true);
    }
    send_checksum();
}

ISR(USARTD0_DRE_vect)
{
    if (!buffer_len()) {
        disable_dre();
    } else {
        LOOP_USART.DATA = buffer_pop();
    }
}
