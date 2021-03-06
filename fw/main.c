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

    cal_init();
    adc_init();

    vdac_init();
    idac_init();

    timers_init();
    timer_pwmset_prereg(0);
    timer_pwmset_ovpth(255);

    loop_init();

    uint8_t adc_channels = adc_n_ch();
    uint8_t channels_scanned = 0;

    for(;;) {
        cal_run();
        if (adc_cycle()) {
            ++channels_scanned;
        }
        if (channels_scanned == adc_channels) {
            channels_scanned = 0;
            psu_fast_cycle();
        }
        if (TICK) {
            TICK = false;
            psu_slow_cycle();
        }
    }
}
