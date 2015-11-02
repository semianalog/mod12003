#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdint.h>
#include <ctype.h>

#include <afw/system.h>
#include <afw/pins.h>
#include <afw/twi.h>
#include <afw/uart.h>

#include "hardware.h"
#include "timers.h"
#include "loop.h"
#include "cal.h"
#include "psu.h"

int main(void)
{
    afw_clock_rc();
    afw_pins_init();
    afw_int_enable(0);

    adc_init();

    vdac_init();
    vdac_set(0x4000uL);

    PSET(P_ISET);   // full current for now
    PSET(P_LINREG_EN);

    timers_init();
    timer_pwmset_ovpth(255);
    timer_pwmset_prereg(255);

    loop_init();

    psu_vset(5000u);

    for(;;) {
        PTGL(P_LEDCV);
        cal_run();
        adc_cycle();
    }
}
