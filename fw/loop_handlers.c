#include <stdlib.h>
#include <avr/pgmspace.h>
#include "loop.h"
#include "config.h"

static void cmd_nop()
{
    send_cmd(LOOP_ADDR_RESPONSE, CMD_ACK, NULL, 0);
}

static void cmd_idn()
{
    send_cmd_P(LOOP_ADDR_RESPONSE, CMD_ACK, (const uint8_t *) PSTR(IDN_STR), sizeof(IDN_STR) - 1);
}

void (* const CMD_HANDLERS[256])() PROGMEM = {
    [CMD_NOP] = &cmd_nop,
    [CMD_IDN] = &cmd_idn,
    [CMD_ACK] = &cmd_nop,
};
