#include "cal.h"
#include "loop.h"
#include "hardware.h"
#include <afw/misc.h>
#include <afw/pins.h>
#include <string.h>
#include <util/delay.h>
#include <util/atomic.h>

uint32_t EEMEM  CAL_C_VSLOPE;
uint16_t EEMEM  CAL_C_VOFFSET;

volatile enum cal_cmd       g_cal_cmd = CAL_NONE;
volatile struct cal_status  g_cal_status = {CAL_STATE_IDLE, {0}, 0};
volatile uint8_t            g_selected_cal = 0xff;
volatile int32_t            g_user_data = 0;
volatile uint8_t*           g_cal_data = NULL;
volatile size_t             g_cal_data_sz = 0;


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

/**
 * Try the general read,write,abort,save actions.
 * @param data - data to read/write
 * @parma sz - size of data
 * @return the cal command that triggered an exit
 */
static enum cal_cmd try_actions(void)
{
    for (;;) {
        enum cal_cmd cmd = g_cal_cmd;
        switch (cmd) {
        case CAL_RUN:
        case CAL_SAVE:
            return cmd;
        case CAL_ABORT:
            cal_finish();
            return cmd;
        default:
            ;//continue
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
    const uint16_t SETPOINT_LO = 0x2000u;
    const uint16_t SETPOINT_HI = 0xd000u;
    const int64_t SETPOINT_DELTA = (uint64_t)(SETPOINT_HI - SETPOINT_LO);

    union {
        struct {
            uint32_t slope;
            int16_t offset;
        };
        uint8_t data[6];
    } cal = {
        { eeprom_read_dword(&CAL_C_VSLOPE),
          eeprom_read_word(&CAL_C_VOFFSET) }
    };

    g_cal_data = &cal.data[0];
    g_cal_data_sz = sizeof(cal.data);

    switch (try_actions()) {
    case CAL_RUN:
        break;
    case CAL_ABORT:
    default:
        return;
    }

    // Running - simulate :)
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        g_cal_status.state = CAL_STATE_RUNNING;
        g_cal_status.msg_len = 0;
    }

    vdac_set(SETPOINT_LO);
    if (get_user_data(FSTR("Measure open ckt"), FSTR("mV"))) return;
    uint16_t measured_lo_mv = g_user_data;

    vdac_set(SETPOINT_HI);
    if (get_user_data(FSTR("Measure open ckt"), FSTR("mV"))) return;
    uint16_t measured_hi_mv = g_user_data;

    // Compute the calibration constants
    int64_t slope_i64 = (INT64_C(65536) * SETPOINT_DELTA) / (measured_hi_mv - measured_lo_mv);
    int64_t offset_i64 = SETPOINT_LO - slope_i64 * measured_lo_mv / INT64_C(65536);

    cal.slope = (uint64_t)slope_i64;
    cal.offset = offset_i64;

    // Finished
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        g_cal_status.state = CAL_STATE_FINISHED;
        g_cal_status.msg_len = 0;
    }


    switch (try_actions()) {
    case CAL_SAVE:
        break;
    case CAL_RUN:
        break;
    case CAL_ABORT:
    default:
        return;
    }

    eeprom_write_dword(&CAL_C_VSLOPE, cal.slope);
    eeprom_write_word(&CAL_C_VOFFSET, cal.offset);

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
