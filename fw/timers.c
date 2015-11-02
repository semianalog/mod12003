#include <avr/io.h>
#include <avr/interrupt.h>
#include "timers.h"

volatile bool TICK = false;

void timers_init(void)
{
    // TCC4 is used to generate PWM, at 62.5 kHz
    // TCC4's overflow bit chains into TCC5's clock
    // TCC5 is used to generate ticks at 1 Hz

    TCC4.CTRLA = TC45_CLKSEL_DIV2_gc;
    TCC4.CTRLB = TC45_WGMODE_SINGLESLOPE_gc;
    TCC4.CTRLE = TC45_CCBMODE_COMP_gc | TC45_CCCMODE_COMP_gc;
    TCC4.PER = 255;
    TCC4.CCB = 0;
    TCC4.CCC = 0;
    TCC4.CTRLGCLR = TC4_STOP_bm;

    EVSYS.CH0MUX = EVSYS_CHMUX_TCC4_OVF_gc;

    TCC5.CTRLA = TC45_CLKSEL_EVCH0_gc;
    TCC5.PER = 15625u;
    TCC5.CTRLGCLR = TC5_STOP_bm;
    TCC5.INTCTRLA = TC45_OVFINTLVL_LO_gc;
}

ISR(TCC5_OVF_vect)
{
    TICK = true;
    TCC5.INTFLAGS |= TC5_OVFIF_bm;
}

void timer_pwmset_ovpth(uint8_t n)
{
    TCC4.CCB = n;
}

void timer_pwmset_prereg(uint8_t n)
{
    TCC4.CCC = n;
}
