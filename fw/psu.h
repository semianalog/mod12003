#ifndef _PSU_H
#define _PSU_H 1

/**
 * Execute one step of the PSU control loop. Call this repeatedly from the
 * main loop.
 */
void psu_control_step(void);

#endif // _PSU_H
