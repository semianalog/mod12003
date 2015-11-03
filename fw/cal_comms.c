#include "cal.h"
#include "loop.h"
#include <afw/misc.h>
#include <stdlib.h>

void cmd_cal_count(void)
{
    uint8_t buffer[2] = { READ_U16_BYTE(N_CAL_ROUTINES, 0), READ_U16_BYTE(N_CAL_ROUTINES, 1) };
    send_msg(LOOP_ADDR_RESPONSE, CMD_ACK, buffer, 2);
}

void cmd_cal_select(void)
{
    uint8_t selection = g_loop_msg.data[0];

    if (selection < N_CAL_ROUTINES) {
        g_selected_cal = selection;
        g_cal_status.state = CAL_STATE_SELECTED;
        g_cal_status.msg_len = 0;
        g_cal_cmd = CAL_NONE;
        send_ack();
    } else {
        send_msg(LOOP_ADDR_RESPONSE, CMD_NACK_DATA, NULL, 0);
    }
}

void cmd_cal_name(void)
{
    const char __flash * name = CAL_NAMES[g_selected_cal];
    size_t len = strlen_P(name);
    send_msg_F(LOOP_ADDR_RESPONSE, CMD_ACK, name, len);
}

void cmd_cal_status(void)
{
    if (g_selected_cal < N_CAL_ROUTINES) {
        send_msg(LOOP_ADDR_RESPONSE, CMD_ACK, &g_cal_status, g_cal_status.msg_len + 1);
    } else {
        uint8_t buffer[1] = {0x00};
        send_msg(LOOP_ADDR_RESPONSE, CMD_ACK, buffer, 1);
    }
}

void cmd_cal_user(void)
{
    uint32_t user_data_u32 = U8_to_U32(
            g_loop_msg.data[0], g_loop_msg.data[1], g_loop_msg.data[2], g_loop_msg.data[3]);
    g_user_data = (int32_t) user_data_u32;
    g_cal_cmd = CAL_USER;
    send_ack();
}

void cmd_cal_run(void)
{
    g_cal_cmd = CAL_RUN;
    send_ack();
}

void cmd_cal_save(void)
{
    g_cal_cmd = CAL_SAVE;
    send_ack();
}

void cmd_cal_abort(void)
{
    g_cal_cmd = CAL_ABORT;
    send_ack();
}

void cmd_cal_read(void)
{
    send_msg(LOOP_ADDR_RESPONSE, CMD_ACK, g_cal_data, g_cal_data_sz);
}

void cmd_cal_write(void)
{
    g_cal_cmd = CAL_WRITE;
    send_ack();
}

