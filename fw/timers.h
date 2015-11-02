#ifndef _TIMERS_H
#define _TIMERS_H

#include <stdbool.h>

extern volatile bool TICK;

void timers_init(void);

void timer_pwmset_ovpth(uint8_t n);
void timer_pwmset_prereg(uint8_t n);

#endif // _TIMERS_H
