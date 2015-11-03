#ifndef _CAL_H
#define _CAL_H

#include <inttypes.h>
#include <avr/eeprom.h>

/******************************************************************************
 * Calibration constants
 */

struct cal_data {
    int32_t     dacslope;
    int16_t     dacoffset;
    int32_t     adcslope;
    int16_t     adcoffset;
};

extern struct cal_data EEMEM    EE_CAL_DATA_VOLTAGE;   // 191069, 102, 84213, 16
extern struct cal_data EEMEM    EE_CAL_DATA_CURRENT;   // 89115, 9, 6311, -124

extern struct cal_data      CAL_DATA_VOLTAGE;
extern struct cal_data      CAL_DATA_CURRENT;

/******************************************************************************
 * Types used internally by the calibration system
 */

/**
 * Calibration commands passed from comms to main loop.
 */
enum cal_cmd {
    CAL_NONE,
    CAL_RUN,
    CAL_SAVE,
    CAL_ABORT,
    CAL_READ,
    CAL_WRITE,
    CAL_USER,
} __attribute__((packed));

// Single-byte accesses are atomic, so make sure the enum is a byte
_Static_assert(sizeof(enum cal_cmd) == 1, "enum cal_cmd must have fewer than 256 elements");

/**
 * Calibration state passed from main loop to comms.
 */
enum cal_state {
    CAL_STATE_IDLE,
    CAL_STATE_SELECTED,
    CAL_STATE_ERROR,
    CAL_STATE_RUNNING,
    CAL_STATE_QUERY,
    CAL_STATE_FINISHED,
} __attribute__((packed));

// This is passed in a defined binary message, so make sure the size is right
_Static_assert(sizeof(enum cal_state) == 1, "enum cal_state must have fewer than 256 elements");

/**
 * Status message passed from main loop to comms.
 */
struct cal_status {
    enum cal_state state;
    uint8_t msg[20];
    uint8_t msg_len;
} __attribute__((packed));

/**
 * Length of cal_status.msg reserved for user data info
 */
#define USER_DATA_INFO_LEN  16

/**
 * Length of cal_status.msg reserved for user data unit
 */
#define USER_DATA_UNIT_LEN  4

_Static_assert(USER_DATA_INFO_LEN + USER_DATA_UNIT_LEN <= sizeof(((struct cal_status){0}).msg),
        "User data message must fit in message field");

/**
 * Calibration routine function
 */
typedef void (*cal_routine_fp)(void);

/******************************************************************************
 * Communication between loop comm ISR and the calibration function in the
 * main loop.
 */

extern volatile enum cal_cmd        g_cal_cmd;
extern volatile struct cal_status   g_cal_status;
extern volatile uint8_t             g_selected_cal;
extern volatile int32_t             g_user_data;
extern volatile uint8_t*            g_cal_data;
extern volatile size_t              g_cal_data_sz;

extern const uint8_t                N_CAL_ROUTINES;
extern const __flash char *         CAL_NAMES[];
extern const __flash cal_routine_fp CAL_FUNCTIONS[];

/******************************************************************************
 * Main loop functions
 */

/**
 * Load calibration data.
 */
void cal_init(void);

/**
 * Run calibration, if the calibration system has been activated, or else return.
 */
void cal_run(void);

/******************************************************************************
 * Comms functions. These run from within the comm loop ISR.
 */

void cmd_cal_count(void);
void cmd_cal_select(void);
void cmd_cal_name(void);
void cmd_cal_status(void);
void cmd_cal_user(void);
void cmd_cal_run(void);
void cmd_cal_save(void);
void cmd_cal_abort(void);
void cmd_cal_read(void);
void cmd_cal_write(void);

#endif // _CAL_H
