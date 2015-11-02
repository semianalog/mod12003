#include "cal.h"
#include "loop.h"
#include "hardware.h"
#include <afw/misc.h>
#include <afw/pins.h>
#include <string.h>
#include <util/delay.h>
#include <util/atomic.h>

volatile enum cal_cmd       g_cal_cmd = CAL_NONE;
volatile struct cal_status  g_cal_status = {CAL_STATE_IDLE, {0}, 0};
volatile uint8_t            g_selected_cal = 0xff;
volatile int32_t            g_user_data = 0;


static void cal_finish(void)
{
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        g_cal_cmd = CAL_NONE;
        g_cal_status.state = CAL_STATE_IDLE;
        g_cal_status.msg_len = 0;
        g_selected_cal = 0xff;
    }
}

static bool check_abort(void)
{
    if (g_cal_cmd == CAL_ABORT) {
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
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        strncpy_P((char *) g_cal_status.msg, info, 16);
        strncpy_P((char *) g_cal_status.msg + 16, unit, 4);
        g_cal_status.msg_len = 20;
        g_cal_status.state = CAL_STATE_QUERY;
    }
    for (;;) {
        if (check_abort()) return true;
        if (g_cal_cmd == CAL_USER) {
            ATOMIC_BLOCK(ATOMIC_FORCEON) {
                g_cal_cmd = CAL_NONE;
                g_cal_status.msg_len = 0;
                g_cal_status.state = CAL_STATE_RUNNING;
            }
            return false;
        }
    }
}

/******************************************************************************
 * Calibration routine: voltage sense
 *
 * Sets two output voltages, asks the user to measure them, and compares to the
 * ADC's measurement.
 */

static const char __flash CAL_NAME_VSENSE[] = "V Sense";
static void CAL_FUNCTION_VSENSE(void)
{
    switch (g_cal_cmd) {
    case CAL_RUN:
        break;
    case CAL_ABORT:
        cal_finish();
        // fall through
    default:
        return;
    }

    // Running - simulate :)
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        g_cal_status.state = CAL_STATE_RUNNING;
        g_cal_status.msg_len = 0;
    }

    vdac_set(0x2000u);
    if (get_user_data(FSTR("Measure open ckt"), FSTR("mV"))) return;

    vdac_set(0xdffcu);
    if (get_user_data(FSTR("Measure open ckt"), FSTR("mV"))) return;

    // Finished
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        g_cal_status.state = CAL_STATE_FINISHED;
        g_cal_status.msg_len = 0;
    }

    for (;;) {
        if (check_abort()) return;
        if (g_cal_cmd == CAL_SAVE) break;
    }

    cal_finish();
}

/**
 * Run calibration. Dispatches to a cal routine function if one is selected.
 */
void cal_run(void)
{
    if (g_selected_cal < N_CAL_ROUTINES) {
        CAL_FUNCTIONS[g_selected_cal]();
    }
}

const __flash char *CAL_NAMES[] = {
    CAL_NAME_VSENSE,
};

const __flash cal_routine_fp CAL_FUNCTIONS[] = {
    CAL_FUNCTION_VSENSE,
};

const uint8_t N_CAL_ROUTINES = sizeof(CAL_FUNCTIONS)/sizeof(CAL_FUNCTIONS[0]);
