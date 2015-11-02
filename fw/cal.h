#ifndef _CAL_H
#define _CAL_H

#include <inttypes.h>

/**
 * Pump the calibration state machine. Run from the main loop.
 */
void cal_state_cycle(void);

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
