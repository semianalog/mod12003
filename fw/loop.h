#ifndef _LOOP_H
#define _LOOP_H

#include "config.h"
#include <afw/misc.h>
#include <inttypes.h>
#include <avr/pgmspace.h>
#include <stdbool.h>
#include <afw/uart.h>

//  Interface code for MOD1 bus loop

// When a message is received bearing data, the data is buffered.
#define LOOP_DATA_BUF_SIZE  64

// Reserved addresses
#define LOOP_ADDR_RESPONSE  254
#define LOOP_ADDR_BROADCAST 255

#define LOOP_RXCINTLVL 1
#define LOOP_DREINTLVL 2


#define CMD_NOP         0x00    // NOP
#define CMD_IDN         0x01    // return model number/identifier string
#define CMD_SERIAL      0x02    // return serial number string - must be unique for same IDN
#define CMD_COUNT       0x10    // see CMD_COUNT below
#define CMD_ACK         0x70
#define CMD_NACK_BUFLEN 0x71
#define CMD_NACK_CRC    0x72
#define CMD_NACK_NO_CMD 0x73
#define CMD_NACK_DATA   0x74    // bad data given to command

#define CMD_OUTPUT_EN   0x80    // payload is one byte, zero=disable nonzero=enable
#define CMD_SET_VOLTAGE 0x81    // set output voltage. payload is a little-endian int32_t millivolts
#define CMD_SET_CURRENT 0x82    // set output current. payload is a little-endian int32_t microamps
#define CMD_QOUTPUT     0x90    // return actual output mode: 0 = off, 1 = CV, 2 = CC, ff = fault
#define CMD_QVOLTAGE    0x91    // return actual output voltage, little-endian int32_t millivolts
#define CMD_QCURRENT    0x92    // return actual output current, little-endian int32_t microamps

#define CMD_QPREREG     0xa0    // query prereg voltage (for debug)

#define CMD_CAL_COUNT   0xf0    // return the number of cal routines, little-endian uint16_t
#define CMD_CAL_SELECT  0xf1    // in:  LE uint16_t, cal routine ID
                                // select a calibration routine
#define CMD_CAL_NAME    0xf2    // out: ASCII string giving name of routine. 16 chars or fewer
#define CMD_CAL_STATUS  0xf3    // return the current status of the running calibration routine.
                                // out: LE uint8_t status ID, possibly followed by more data
                                //   0: no cal running or selected
                                //   1: cal routine selected but not running
                                //   2: stopped with error (error message follows in ASCII, <=16chars)
                                //   3: running, wait...
                                //   4: paused for user input (data follows, extra chars NUL:
                                //          char[16]    name of requested user input (like OpenCkt Voltage)
                                //          char[8]     unit of requested user input (like mV)  )
                                //   5: success, paused to confirm save constants
#define CMD_CAL_USER    0xf4    // provide user data when requested
#define CMD_CAL_RUN     0xf5    // run the selected routine
#define CMD_CAL_SAVE    0xf6    // confirm saving cal constants of a completed routine
#define CMD_CAL_ABORT   0xf7    // abort routine, do not save constants
#define CMD_CAL_READ    0xf8    // read the cal constants of the selected routine as a binary blob.
                                //  reads constants from EEPROM unless routine has finished; then
                                //  reads the new constants that will be written on CMD_CAL_SAVE
#define CMD_CAL_WRITE   0xf9    // stores new cal constants of the selected routine as a binary blob.
                                //  only stages the constants for write by CMD_CAL_SAVE, does not
                                //  actually write to EEPROM.

/* CMD_COUNT
 * Used to count the number of devices on the loop. This is sent to the first device
 * (address zero), which should then increment the 8-bit number in the data block and
 * send it to the next device. When it comes back around, it will contain the number
 * of devices on the loop.
 */

/* Jump table, one per command */
extern void (* const __flash CMD_HANDLERS[256])();

/**
 * A messsage received on the loop.
 */
struct loop_msg {
    uint8_t addr;
    uint8_t cmd;
    uint16_t datalen;
    uint8_t data[LOOP_DATA_BUF_SIZE];
    uint16_t crc;
};

/**
 * The last message received.
 */
extern volatile struct loop_msg g_loop_msg;

/**
 * Send a message on the loop. Handlers can use this for responses.
 *
 * Warning: If calling from the main loop, MASK INTERRUPTS FIRST! This makes use of the
 * hardware CRC module. Do NOT use this from an interrupt level above LOOP_RXCINTLVL.
 *
 * @param addr - Destination address
 * @param cmd  - Command ID
 * @param data - Block of data to send. Can be NULL iff datalen == 0
 * @param datalen - Length of data to send
 */
void send_msg(uint8_t addr, uint8_t cmd, const volatile void *data, uint16_t datalen);

void send_msg_F(uint8_t addr, uint8_t cmd, const __flash void *data, uint16_t datalen);

/**
 * Shortcut to send an empty ACK message.
 */
void send_ack(void);

/******************************************************************************
 * Low-level functions
 */

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
void buffer_send(uint8_t byte, bool crc);

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
void buffer_send_bytes(const uint8_t *data, size_t len, bool crc);

UNUSED( static void loop_init(void) )
{
    LOOP_UART.init( /* txen */ true, /* rxen */ true, 8, 'N', 1 );
    LOOP_UART.rxcint(LOOP_RXCINTLVL);
}

#endif // _LOOP_H
