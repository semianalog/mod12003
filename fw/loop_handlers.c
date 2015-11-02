#include "config.h"
#include "loop.h"
#include "hardware.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include <afw/misc.h>

#include "cal.h"

static void cmd_nop()
{
    send_msg(LOOP_ADDR_RESPONSE, CMD_ACK, NULL, 0);
}

static void cmd_idn()
{
    send_msg_F(LOOP_ADDR_RESPONSE, CMD_ACK, (const __flash uint8_t *) FSTR(IDN_STR), sizeof(IDN_STR) - 1);
}

static void cmd_serial()
{
    // TODO: maybe CRC this data to get a shorter serial?
    char buffer[45];
    sprintf(buffer, "lot(%02X%02X%02X%02X%02X%02X)waf(%02X)x(%02X%02X)y(%02X%02X)",
            READ_PRODSIG(LOTNUM5),
            READ_PRODSIG(LOTNUM4),
            READ_PRODSIG(LOTNUM3),
            READ_PRODSIG(LOTNUM2),
            READ_PRODSIG(LOTNUM1),
            READ_PRODSIG(LOTNUM0),
            READ_PRODSIG(WAFNUM),
            READ_PRODSIG(COORDX1),
            READ_PRODSIG(COORDX0),
            READ_PRODSIG(COORDY1),
            READ_PRODSIG(COORDY0));
    send_msg(LOOP_ADDR_RESPONSE, CMD_ACK, (uint8_t *) buffer, strlen(buffer));
}

static void cmd_count()
{
    uint8_t count = g_loop_msg.data[0] + 1;
    send_msg(0, CMD_COUNT, &count, 1);
}

static void cmd_set_voltage(void)
{
    //temporary: just set the DAC word
    uint16_t dac_word = U8_to_U16(g_loop_msg.data);
    vdac_set(dac_word);
    send_msg(LOOP_ADDR_RESPONSE, CMD_ACK, NULL, 0);
}

void (* const CMD_HANDLERS[256])() PROGMEM = {
    [CMD_NOP] = &cmd_nop,
    [CMD_IDN] = &cmd_idn,
    [CMD_SERIAL] = &cmd_serial,
    [CMD_COUNT] = &cmd_count,
    [CMD_ACK] = &cmd_nop,
    [CMD_SET_VOLTAGE] = &cmd_set_voltage,

    [CMD_CAL_COUNT]  = &cmd_cal_count,
    [CMD_CAL_SELECT] = &cmd_cal_select,
    [CMD_CAL_NAME]   = &cmd_cal_name,
    [CMD_CAL_STATUS] = &cmd_cal_status,
    [CMD_CAL_USER]   = &cmd_cal_user,
    [CMD_CAL_RUN]    = &cmd_cal_run,
    [CMD_CAL_SAVE]   = &cmd_cal_save,
    [CMD_CAL_ABORT]  = &cmd_cal_abort,
    [CMD_CAL_READ]   = &cmd_cal_read,
    [CMD_CAL_WRITE]  = &cmd_cal_write,
};
