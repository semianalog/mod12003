#include "cal.h"
#include "loop.h"
#include "dac.h"
#include <afw/misc.h>
#include <afw/pins.h>
#include <string.h>
#include <util/delay.h>

enum cal_cmd {
    CAL_NONE,
    CAL_RUN,
    CAL_SAVE,
    CAL_ABORT,
    CAL_READ,
    CAL_WRITE,
    CAL_USER,
};

static volatile uint8_t gs_cal_cmd = CAL_NONE;

static volatile struct {
    uint8_t status;
    uint8_t msg[20];
    uint8_t msg_len;
} gs_cal_status = {0, {0}, 0};

static volatile uint8_t gs_selected_cal = 0xff;

static volatile int32_t gs_user_data = 0;


static void cal_finish(void)
{
    cli();
    gs_cal_cmd = CAL_NONE;
    gs_cal_status.status = 0;
    gs_cal_status.msg_len = 0;
    gs_selected_cal = 0xff;
    sei();
}

static bool check_abort(void)
{
    if (gs_cal_cmd == CAL_ABORT) {
        cal_finish();
        return true;
    } else {
        return false;
    }
}

/**
 * true on abort
 */
static bool get_user_data(const __flash char *info, const __flash char *unit)
{
    cli();
    strncpy_P((char *) gs_cal_status.msg, info, 16);
    strncpy_P((char *) gs_cal_status.msg + 16, unit, 4);
    gs_cal_status.msg_len = 20;
    gs_cal_status.status = 4;
    sei();
    for (;;) {
        if (check_abort()) return true;
        if (gs_cal_cmd == CAL_USER) {
            cli();
            gs_cal_cmd = CAL_NONE;
            gs_cal_status.msg_len = 0;
            gs_cal_status.status = 3;
            sei();
            return false;
        }
    }
}

const char __flash CAL_NAME_0[] = "V Sense";
static void CAL_FXN_0(void)
{
    switch (gs_cal_cmd) {
    case CAL_RUN:
        break;
    default:
        return;
    }

    // Running - simulate :)
    cli();
    gs_cal_status.status = 3;
    gs_cal_status.msg_len = 0;
    sei();

    maxdac_set(0x2000u);
    if (get_user_data(FSTR("Measure open ckt"), FSTR("mV"))) return;

    maxdac_set(0xdffcu);
    if (get_user_data(FSTR("Measure open ckt"), FSTR("mV"))) return;

    // Finished
    cli();
    gs_cal_status.status = 5;
    gs_cal_status.msg_len = 0;
    sei();

    for (;;) {
        if (check_abort()) return;
        if (gs_cal_cmd == CAL_SAVE) break;
    }

    cal_finish();
}

const __flash char *CAL_NAMES[] = {
    CAL_NAME_0,
};

const uint16_t N_CALS = 1;
void (* const __flash CAL_FXNS[])(void) = {
    CAL_FXN_0,
};


void cal_state_cycle(void)
{
    if (gs_selected_cal < N_CALS) {
        CAL_FXNS[gs_selected_cal]();
    }
}


void cmd_cal_count(void)
{
    char buffer[2];
    U8_to_U16(buffer) = N_CALS;
    send_msg(LOOP_ADDR_RESPONSE, CMD_ACK, (uint8_t *) buffer, 2);
}

void cmd_cal_select(void)
{
    uint8_t selection = g_loop_msg.data[0];

    if (selection < N_CALS) {
        gs_selected_cal = selection;
        gs_cal_status.status = 1;
        gs_cal_status.msg_len = 0;
        gs_cal_cmd = CAL_NONE;
        send_msg_F(LOOP_ADDR_RESPONSE, CMD_ACK, (uint8_t *) FSTR("Yo!"), 3);
    } else {
        send_msg(LOOP_ADDR_RESPONSE, CMD_NACK_DATA, NULL, 0);
    }
}

void cmd_cal_name(void)
{
    const char __flash * name = CAL_NAMES[gs_selected_cal];
    size_t len = strlen_P(name);
    send_msg_F(LOOP_ADDR_RESPONSE, CMD_ACK, (const __flash uint8_t *) name, len);
}

void cmd_cal_status(void)
{
    if (gs_selected_cal < N_CALS) {
        send_msg(LOOP_ADDR_RESPONSE, CMD_ACK, (uint8_t *) &gs_cal_status, gs_cal_status.msg_len + 1);
    } else {
        uint8_t buffer[1] = {0x00};
        send_msg(LOOP_ADDR_RESPONSE, CMD_ACK, buffer, 1);
    }
}

void cmd_cal_user(void)
{
    gs_user_data = *(int32_t *)(&g_loop_msg.data);
    gs_cal_cmd = CAL_USER;
    send_msg(LOOP_ADDR_RESPONSE, CMD_ACK, NULL, 0);
}

void cmd_cal_run(void)
{
    gs_cal_cmd = CAL_RUN;
    send_msg(LOOP_ADDR_RESPONSE, CMD_ACK, NULL, 0);
}

void cmd_cal_save(void)
{
    gs_cal_cmd = CAL_SAVE;
    send_msg(LOOP_ADDR_RESPONSE, CMD_ACK, NULL, 0);
}

void cmd_cal_abort(void)
{
    gs_cal_cmd = CAL_ABORT;
    send_msg(LOOP_ADDR_RESPONSE, CMD_ACK, NULL, 0);
}

void cmd_cal_read(void)
{
    gs_cal_cmd = CAL_READ;
    send_msg(LOOP_ADDR_RESPONSE, CMD_ACK, NULL, 0);
}

void cmd_cal_write(void)
{
    gs_cal_cmd = CAL_WRITE;
    send_msg(LOOP_ADDR_RESPONSE, CMD_ACK, NULL, 0);
}

