#include "cal.h"
#include "loop.h"
#include "hardware.h"
#include "psu.h"
#include "misc_math.h"
#include <afw/misc.h>
#include <afw/pins.h>
#include <string.h>
#include <util/delay.h>
#include <util/atomic.h>

struct cal_data EEMEM   EE_CAL_DATA_VOLTAGE;
struct cal_data EEMEM   EE_CAL_DATA_CURRENT;

struct cal_data         CAL_DATA_VOLTAGE;
struct cal_data         CAL_DATA_CURRENT;

#define NO_SELECTED_CAL 0xff

volatile enum cal_cmd       g_cal_cmd = CAL_NONE;
volatile struct cal_status  g_cal_status = {CAL_STATE_IDLE, {0}, 0};
volatile uint8_t            g_selected_cal = NO_SELECTED_CAL;
volatile int32_t            g_user_data = 0;
volatile uint8_t*           g_cal_data = NULL;
volatile size_t             g_cal_data_sz = 0;


static void cal_finish(void)
{
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        g_cal_cmd = CAL_NONE;
        g_cal_status.state = CAL_STATE_IDLE;
        g_cal_status.msg_len = 0;
        g_selected_cal = NO_SELECTED_CAL;
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
        strncpy_P((char *) g_cal_status.msg, info, USER_DATA_INFO_LEN);
        strncpy_P((char *) g_cal_status.msg + USER_DATA_INFO_LEN, unit, USER_DATA_UNIT_LEN);
        g_cal_status.msg_len = USER_DATA_INFO_LEN + USER_DATA_UNIT_LEN;
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

/**
 * Parameters for generic_cal, kept in progmem
 */
struct cal_config {
    uint16_t setpoint_lo;
    uint16_t setpoint_hi;
    struct cal_data *ee_cal_data;
    struct cal_data *cal_data;
    void (*set_value)(uint16_t);
    void (*initialize)(void);
    char user_msg[16];
    char user_unit[4];
    uint8_t adc_channel;
};

/**
 * Generic calibration routine, parametized for each function
 */
static void generic_cal(const __flash struct cal_config *cfg)
{
    int64_t setpoint_delta = (uint64_t)(cfg->setpoint_hi - cfg->setpoint_lo);

    eeprom_read_block(cfg->cal_data, cfg->ee_cal_data, sizeof(struct cal_data));

    struct cal_data *cal = cfg->cal_data;

    g_cal_data = (uint8_t *) cfg->cal_data;
    g_cal_data_sz = sizeof(struct cal_data);

    switch (try_actions()) {
    case CAL_RUN:
        break;
    case CAL_ABORT:
    default:
        return;
    }

    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        g_cal_status.state = CAL_STATE_RUNNING;
        g_cal_status.msg_len = 0;
    }

    cfg->initialize();
    cfg->set_value(cfg->setpoint_lo);
    if (get_user_data(cfg->user_msg, cfg->user_unit)) return;
    uint16_t measured_lo = g_user_data;
    uint16_t adc_lo = (uint16_t)(adc_sample_n(cfg->adc_channel, 1024) / 1024);

    cfg->set_value(cfg->setpoint_hi);
    if (get_user_data(cfg->user_msg, cfg->user_unit)) return;
    uint16_t measured_hi = g_user_data;
    uint16_t adc_hi = (uint16_t)(adc_sample_n(cfg->adc_channel, 1024) / 1024);

    uint16_t measured_delta = measured_hi - measured_lo;
    uint16_t adc_delta = adc_hi - adc_lo;

    int32_t dacslope_i64 = (LINEAR_DENOM * setpoint_delta) / measured_delta;
    int32_t dacoffset_i64 = cfg->setpoint_lo - dacslope_i64 * measured_lo / LINEAR_DENOM;

    int32_t adcslope_i64 = (LINEAR_DENOM * measured_delta) / adc_delta;
    int32_t adcoffset_i64 = measured_lo - adcslope_i64 * adc_lo / LINEAR_DENOM;

    cal->dacslope = dacslope_i64;
    cal->dacoffset = dacoffset_i64;
    cal->adcslope = adcslope_i64;
    cal->adcoffset = adcoffset_i64;

    // Finished
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        g_cal_status.state = CAL_STATE_FINISHED;
        g_cal_status.msg_len = 0;
    }

    switch (try_actions()) {
    case CAL_SAVE:
    case CAL_RUN:
        break;
    case CAL_ABORT:
    default:
        return;
    }

    eeprom_write_block(cfg->cal_data, cfg->ee_cal_data, sizeof(struct cal_data));

    cal_finish();
}

/******************************************************************************
 * Calibration routine: voltage
 *
 * Sets two output voltages, asks the user to measure them, and compares to the
 * ADC's measurement.
 */

// ADD_GRAPHED_CALL generic_cal voltage_set_value
// ADD_GRAPHED_CALL generic_cal voltage_initialize
static void voltage_set_value(uint16_t v) {
    vdac_set(v);
}
static void voltage_initialize(void) {
    idac_set(64u);
    psu_prereg_vset(25000);
}
static const char __flash CAL_NAME_VOLTAGE[] = "Voltage";
static void CAL_FUNCTION_VOLTAGE(void)
{
    static const __flash struct cal_config cfg = {
        .setpoint_lo = 0x2000u,
        .setpoint_hi = 0xd000u,
        .ee_cal_data       = &EE_CAL_DATA_VOLTAGE,
        .cal_data          = &CAL_DATA_VOLTAGE,
        .set_value = &voltage_set_value,
        .initialize = &voltage_initialize,
        .user_msg = "Measure open ckt",
        .user_unit = "mV",
        .adc_channel = ADC_VSENSE,
    };
    generic_cal(&cfg);
}

/******************************************************************************
 * Calibration routine: current
 *
 * Sets two output currents, asks the user to measure them, and compares to the
 * ADC's measurement.
 */

// ADD_GRAPHED_CALL generic_cal current_set_value
// ADD_GRAPHED_CALL generic_cal current_initialize
static void current_set_value(uint16_t v) {
    idac_set(v);
}
static void current_initialize(void) {
    psu_prereg_vset(2500);
    vdac_set(0x1000);
}
static const char __flash CAL_NAME_CURRENT[] = "Current";
static void CAL_FUNCTION_CURRENT(void)
{
    // XXX: raise SETPOINT_HI - it's low now because I'm testing using a limited
    // input power source

    static const __flash struct cal_config cfg = {
        .setpoint_lo = 64u << 1,    // about 50mA
        .setpoint_hi = 321u << 1,   // about 250mA
        .ee_cal_data       = &EE_CAL_DATA_CURRENT,
        .cal_data          = &CAL_DATA_CURRENT,
        .set_value = &current_set_value,
        .initialize = &current_initialize,
        .user_msg = "Measure shrt ckt",
        .user_unit = "mA",
        .adc_channel = ADC_ISENSE,
    };
    generic_cal(&cfg);
}


void cal_run(void)
{
    uint8_t cal = g_selected_cal;
    if (cal < N_CAL_ROUTINES) {
        CAL_FUNCTIONS[cal]();
    }
}

void cal_init(void)
{
    eeprom_read_block(&CAL_DATA_VOLTAGE, &EE_CAL_DATA_VOLTAGE, sizeof(struct cal_data));
    eeprom_read_block(&CAL_DATA_CURRENT, &EE_CAL_DATA_CURRENT, sizeof(struct cal_data));
}

const __flash char *CAL_NAMES[] = {
    CAL_NAME_VOLTAGE,
    CAL_NAME_CURRENT,
};

// ADD_GRAPHED_CALL cal_run CAL_FUNCTION_.*
const __flash cal_routine_fp CAL_FUNCTIONS[] = {
    &CAL_FUNCTION_VOLTAGE,
    &CAL_FUNCTION_CURRENT,
};

const uint8_t N_CAL_ROUTINES = sizeof(CAL_FUNCTIONS)/sizeof(CAL_FUNCTIONS[0]);
