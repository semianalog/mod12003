#include "cal.h"
#include "loop.h"
#include "hardware.h"
#include "psu.h"
#include <afw/misc.h>
#include <afw/pins.h>
#include <string.h>
#include <util/delay.h>
#include <util/atomic.h>

uint32_t EEMEM  CAL_C_VDACSLOPE_NUMER;
uint16_t EEMEM  CAL_C_VDACOFFSET;
uint32_t EEMEM  CAL_C_VADCSLOPE_NUMER;
uint16_t EEMEM  CAL_C_VADCOFFSET;
uint32_t EEMEM  CAL_C_IDACSLOPE_NUMER;
uint16_t EEMEM  CAL_C_IDACOFFSET;
uint32_t EEMEM  CAL_C_IADCSLOPE_NUMER;
uint16_t EEMEM  CAL_C_IADCOFFSET;

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
    uint32_t *c_dacslope_numer;
    uint16_t *c_dacoffset;
    uint32_t *c_adcslope_numer;
    uint16_t *c_adcoffset;
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

    union {
        struct {
            uint32_t dacslope;
            int16_t dacoffset;
            uint32_t adcslope;
            int16_t adcoffset;
        };
        uint8_t data[12];
    } cal = {
        { eeprom_read_dword(cfg->c_dacslope_numer),
          eeprom_read_word(cfg->c_dacoffset),
          eeprom_read_dword(cfg->c_adcslope_numer),
          eeprom_read_word(cfg->c_adcoffset) }
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

    int64_t dacslope_i64 = (CAL_C_DENOM * setpoint_delta) / measured_delta;
    int64_t dacoffset_i64 = cfg->setpoint_lo - dacslope_i64 * measured_lo / CAL_C_DENOM;

    int64_t adcslope_i64 = (CAL_C_DENOM * measured_delta) / adc_delta;
    int64_t adcoffset_i64 = measured_lo - adcslope_i64 * adc_lo / CAL_C_DENOM;

    cal.dacslope = (uint64_t) dacslope_i64;
    cal.dacoffset = dacoffset_i64;
    cal.adcslope = (uint64_t) adcslope_i64;
    cal.adcoffset = adcoffset_i64;

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

    eeprom_write_dword(cfg->c_dacslope_numer, cal.dacslope);
    eeprom_write_word(cfg->c_dacoffset, cal.dacoffset);
    eeprom_write_dword(cfg->c_adcslope_numer, cal.adcslope);
    eeprom_write_word(cfg->c_adcoffset, cal.adcoffset);

    cal_finish();
}

/******************************************************************************
 * Calibration routine: voltage
 *
 * Sets two output voltages, asks the user to measure them, and compares to the
 * ADC's measurement.
 */

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
        .c_dacslope_numer  = &CAL_C_VDACSLOPE_NUMER,
        .c_dacoffset       = &CAL_C_VDACOFFSET,
        .c_adcslope_numer  = &CAL_C_VADCSLOPE_NUMER,
        .c_adcoffset       = &CAL_C_VADCOFFSET,
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
        .c_dacslope_numer  = &CAL_C_IDACSLOPE_NUMER,
        .c_dacoffset       = &CAL_C_IDACOFFSET,
        .c_adcslope_numer  = &CAL_C_IADCSLOPE_NUMER,
        .c_adcoffset       = &CAL_C_IADCOFFSET,
        .set_value = &current_set_value,
        .initialize = &current_initialize,
        .user_msg = "Measure shrt ckt",
        .user_unit = "mA",
        .adc_channel = ADC_ISENSE,
    };
    generic_cal(&cfg);
}


/**
 * Run calibration. Dispatches to a cal routine function if one is selected.
 */
void cal_run(void)
{
    uint8_t cal = g_selected_cal;
    if (cal < N_CAL_ROUTINES) {
        CAL_FUNCTIONS[cal]();
    }
}

const __flash char *CAL_NAMES[] = {
    CAL_NAME_VOLTAGE,
    CAL_NAME_CURRENT,
};

const __flash cal_routine_fp CAL_FUNCTIONS[] = {
    &CAL_FUNCTION_VOLTAGE,
    &CAL_FUNCTION_CURRENT,
};

const uint8_t N_CAL_ROUTINES = sizeof(CAL_FUNCTIONS)/sizeof(CAL_FUNCTIONS[0]);
