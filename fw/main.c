#include "config.h"

#include <avr/pgmspace.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <ctype.h>
#include <stdio.h>
#include <limits.h>

#include "hardware.h"
#include "loop.h"
#include "usart_stdio.h"
#include "psu.h"

static void configure_clock(void)
{
    // Clock: 32 MHz
    OSC.CTRL |= OSC_RC32MEN_bm;
    while (!(OSC.STATUS & OSC_RC32MRDY_bm));
    CCP = CCP_IOREG_gc;
    CLK.CTRL = CLK_SCLKSEL_RC32M_gc;
    OSC.CTRL &= ~OSC_RC2MEN_bm;
    OSC.CTRL |= OSC_RC32KEN_bm;
    while (!(OSC.STATUS & OSC_RC32KRDY_bm));
    OSC.DFLLCTRL = OSC_RC32MCREF_RC32K_gc;
    DFLLRC32M.CTRL |= DFLL_ENABLE_bm;
}

static void configure_pins(void)
{
    PORTA.DIR = 0;
    PORTC.DIR = 0;
    PORTD.DIR = 0;
    PORTR.DIR = 0;

#   define D_IN 0
#   define D_OUT 1
#   define X(port, pin, name, dir, defval, drive)       \
    if (dir == D_OUT)   PORT##port.DIR |= (1 << (pin)); \
    PORT##port.PIN##pin##CTRL = drive;                  \
    if (defval) PORT##port.OUT |= (1 << (pin));         \
    else PORT##port.OUT &= ~(1 << (pin));

    SYSTEM_CONF_PINS;

#   undef X
#   undef OUT
#   undef IN
}


int main(void)
{

    configure_clock();
    configure_pins();
    configure_i2c();
    configure_i2cdac();

    configure_dac();
    configure_adc();

    configure_loop();

    PMIC.CTRL = 0
        | PMIC_RREN_bm
        | PMIC_MEDLVLEN_bm
        | PMIC_LOLVLEN_bm;
    sei();

    for(;;) {
        psu_control_step();
    }
}
