#include "cal.h"
#include "loop.h"
#include <afw/misc.h>
#include <stdlib.h>

void cmd_cal_count(void)
{
    char buffer[2];
    U8_to_U16(buffer) = N_CAL_ROUTINES;
    send_msg(LOOP_ADDR_RESPONSE, CMD_ACK, (uint8_t *) buffer, 2);
}

void cmd_cal_select(void)
{
    uint8_t selection = g_loop_msg.data[0];

    if (selection < N_CAL_ROUTINES) {
        g_selected_cal = selection;
        g_cal_status.state = CAL_STATE_SELECTED;
        g_cal_status.msg_len = 0;
        g_cal_cmd = CAL_NONE;
        send_msg_F(LOOP_ADDR_RESPONSE, CMD_ACK, (uint8_t *) FSTR("Yo!"), 3);
    } else {
        send_msg(LOOP_ADDR_RESPONSE, CMD_NACK_DATA, NULL, 0);
    }
}

void cmd_cal_name(void)
{
    const char __flash * name = CAL_NAMES[g_selected_cal];
    size_t len = strlen_P(name);
    send_msg_F(LOOP_ADDR_RESPONSE, CMD_ACK, (const __flash uint8_t *) name, len);
}

void cmd_cal_status(void)
{
    if (g_selected_cal < N_CAL_ROUTINES) {
        send_msg(LOOP_ADDR_RESPONSE, CMD_ACK, (uint8_t *) &g_cal_status, g_cal_status.msg_len + 1);
    } else {
        uint8_t buffer[1] = {0x00};
        send_msg(LOOP_ADDR_RESPONSE, CMD_ACK, buffer, 1);
    }
}

void cmd_cal_user(void)
{
    g_user_data = *(int32_t *)(&g_loop_msg.data);
    g_cal_cmd = CAL_USER;
    send_msg(LOOP_ADDR_RESPONSE, CMD_ACK, NULL, 0);
}

void cmd_cal_run(void)
{
    g_cal_cmd = CAL_RUN;
    send_msg(LOOP_ADDR_RESPONSE, CMD_ACK, NULL, 0);
}

void cmd_cal_save(void)
{
    g_cal_cmd = CAL_SAVE;
    send_msg(LOOP_ADDR_RESPONSE, CMD_ACK, NULL, 0);
}

void cmd_cal_abort(void)
{
    g_cal_cmd = CAL_ABORT;
    send_msg(LOOP_ADDR_RESPONSE, CMD_ACK, NULL, 0);
}

void cmd_cal_read(void)
{
    g_cal_cmd = CAL_READ;
    send_msg(LOOP_ADDR_RESPONSE, CMD_ACK, NULL, 0);
}

void cmd_cal_write(void)
{
    g_cal_cmd = CAL_WRITE;
    send_msg(LOOP_ADDR_RESPONSE, CMD_ACK, NULL, 0);
}

