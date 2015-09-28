#include "config.h"
#include "hardware.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

static const uint8_t PROGMEM ADC_CHANNELS[] = {
    ADC_REFGND, ADC_ISENSE, ADC_VSENSE, ADC_TEMP, ADC_PREREG, 255 };

static volatile uint16_t _ADC_RESULTS[16] = {0};

void configure_dac(void)
{
    // Copy factory calibration
    DACA.CH0GAINCAL = PRODSIGNATURES_DACA0GAINCAL;
    DACA.CH0OFFSETCAL = PRODSIGNATURES_DACA0OFFCAL;

    DACA.CTRLA = DAC_CH0EN_bm | DAC_LPMODE_bm;
    DACA.CTRLC = DAC_REFSEL1_bm;
    DACA.CTRLA |= DAC_ENABLE_bm;
}

void set_iset_dac(uint16_t value)
{
    DACA.CH0DATA = value;
}

void configure_adc(void)
{
    ADCA.CTRLA = 0;
    ADCA.CTRLB = ADC_RESOLUTION_MT12BIT_gc;
    ADCA.REFCTRL = ADC_REFSEL1_bm;  /* port A VREF */
    ADCA.PRESCALER =ADC_PRESCALER_DIV32_gc;
    ADCA.SAMPCTRL = 0x3f;   /* maximum sampling time */
    ADCA.CAL = ((uint16_t) PRODSIGNATURES_ADCACAL1) << 8  |  PRODSIGNATURES_ADCACAL0;
    ADCA.CTRLA = ADC_ENABLE_bm;

    _delay_us(400);

    ADCA.CH0.INTCTRL = 1;   // Low-level interrupt
    ADCA.CH0.CTRL = ADC_CH_GAIN_1X_gc || ADC_CH_INPUTMODE_SINGLEENDED_gc;
    ADCA.CH0.AVGCTRL = ADC_SAMPNUM_8X_gc;
    ADCA.CH0.MUXCTRL = ADC_CH(0);
    ADCA.CH0.CTRL |= ADC_CH_START_bm;
}

uint16_t get_adc_result(uint8_t n)
{
    cli();
    uint16_t hold = _ADC_RESULTS[n];
    sei();
    return hold;
}

#include <stdio.h>
ISR(ADCA_CH0_vect)
{
    static uint8_t channel_idx = 0;
    static uint8_t channel = ADC_REFGND;

    _ADC_RESULTS[channel] = ADCA.CH0RES;

    ++channel_idx;
    channel = pgm_read_byte(&ADC_CHANNELS[channel_idx]);
    if (channel == 255) {
        channel_idx = 0;
        channel = pgm_read_byte(&ADC_CHANNELS[channel_idx]);
    }

    ADCA.CH0.MUXCTRL = ADC_CH(channel);
    ADCA.CH0.CTRL |= ADC_CH_START_bm;
}

uint8_t read_prodsig(uint8_t idx)
{
    uint8_t result;
    cli();
    NVM_CMD = NVM_CMD_READ_CALIB_ROW_gc;
    result = pgm_read_byte(idx);
    NVM_CMD = NVM_CMD_NO_OPERATION_gc;
    sei();
    return result;
}
