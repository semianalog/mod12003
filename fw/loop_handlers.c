#include "config.h"
#include "loop.h"
#include "hardware.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <avr/pgmspace.h>

static void cmd_nop()
{
    send_msg(LOOP_ADDR_RESPONSE, CMD_ACK, NULL, 0);
}

static void cmd_idn()
{
    send_msg_P(LOOP_ADDR_RESPONSE, CMD_ACK, (const uint8_t *) PSTR(IDN_STR), sizeof(IDN_STR) - 1);
}

static void cmd_serial()
{
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

void (* const CMD_HANDLERS[256])() PROGMEM = {
    [CMD_NOP] = &cmd_nop,
    [CMD_IDN] = &cmd_idn,
    [CMD_SERIAL] = &cmd_serial,
    [CMD_COUNT] = &cmd_count,
    [CMD_ACK] = &cmd_nop,
};
