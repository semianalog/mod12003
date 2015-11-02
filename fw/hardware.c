#include "config.h"
#include "hardware.h"

#include <afw/twi.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <util/atomic.h>

#define MAXDAC_TWI AFW_TWIBB

#define MAXDAC_ADDR 0x38

#define MAXDAC_CODE_LOAD    0x01

#define MAXDAC_USER_CONFIG  0x08
#define MAXDAC_CLEAR_DEFAULT_bm 0x00
#define MAXDAC_CLEAR_ZERO_bm    0x10
#define MAXDAC_CLEAR_MID_bm     0x20
#define MAXDAC_CLEAR_FULL_bm    0x30
#define MAXDAC_AUX_DISABLE_bm   0x00
#define MAXDAC_AUX_nLDAC_bm     0x04
#define MAXDAC_AUX_nCLR_bm      0x08
#define MAXDAC_PWR_ON_bm        0x00
#define MAXDAC_PWR_HIZ_bm       0x01
#define MAXDAC_PWR_100k_bm      0x02
#define MAXDAC_PWR_1k_bm        0x03

static const __flash uint8_t ADC_CHANNELS[] = {
    ADC_REFGND, ADC_ISENSE, ADC_VSENSE, ADC_TEMP, ADC_PREREG };

const uint8_t N_ADC_CHANNELS = sizeof(ADC_CHANNELS) / sizeof(ADC_CHANNELS[0]);

static volatile uint16_t gs_adc_results[16] = {0};
static volatile uint8_t gs_adc_channel_idx = 0;

void idac_init(void)
{
    // Copy factory calibration
    DACA.CH0GAINCAL = READ_PRODSIG(DACA0GAINCAL);
    DACA.CH0OFFSETCAL = READ_PRODSIG(DACA0OFFCAL);

    DACA.CTRLA = DAC_CH0EN_bm | DAC_LPMODE_bm;
    DACA.CTRLC = DAC_REFSEL1_bm;
    DACA.CTRLA |= DAC_ENABLE_bm;
}

void idac_set(uint16_t value)
{
    DACA.CH0DATA = value;
}

bool vdac_init(void)
{
    MAXDAC_TWI.init();

    uint8_t buffer[3] = {MAXDAC_USER_CONFIG, 0x00,
        MAXDAC_CLEAR_ZERO_bm | MAXDAC_AUX_DISABLE_bm | MAXDAC_PWR_ON_bm };
    return MAXDAC_TWI.transact(MAXDAC_ADDR, buffer, 3, 0) != TWI_GOOD;
}

bool vdac_set(uint16_t value)
{
    uint8_t buffer[3] = {MAXDAC_CODE_LOAD, U16_BYTE(value, 1), U16_BYTE(value, 0)};
    return MAXDAC_TWI.transact(MAXDAC_ADDR, buffer, 3, 0) != TWI_GOOD;
}

void adc_init(void)
{
    ADCA.CTRLA = 0;
    ADCA.CTRLB = ADC_RESOLUTION_MT12BIT_gc;
    ADCA.REFCTRL = ADC_REFSEL1_bm;  /* port A VREF */
    ADCA.PRESCALER =ADC_PRESCALER_DIV32_gc;
    ADCA.SAMPCTRL = 0x3f;   /* maximum sampling time */
    ADCA.CAL = ((uint16_t) READ_PRODSIG(ADCACAL1)) << 8  |  READ_PRODSIG(ADCACAL0);
    ADCA.CTRLA = ADC_ENABLE_bm;

    _delay_us(400);

    gs_adc_channel_idx = 0;
    ADCA.CH0.CTRL = ADC_CH_GAIN_1X_gc || ADC_CH_INPUTMODE_SINGLEENDED_gc;
    ADCA.CH0.AVGCTRL = ADC_SAMPNUM_8X_gc;
    ADCA.CH0.MUXCTRL = ADC_CH(0);
    ADCA.CH0.INTFLAGS = ADC_CH_IF_bm;
    ADCA.CH0.CTRL |= ADC_CH_START_bm;
}

bool adc_cycle(void)
{
    if (!(ADCA.CH0.INTFLAGS & ADC_CH_IF_bm)) {
        return false;
    }
    ADCA.CH0.INTFLAGS = ADC_CH_IF_bm;

    uint8_t channel = ADC_CHANNELS[gs_adc_channel_idx];

    gs_adc_results[channel] = ADCA.CH0RES;

    gs_adc_channel_idx = (gs_adc_channel_idx + 1) % N_ADC_CHANNELS;

    ADCA.CH0.MUXCTRL = ADC_CH(ADC_CHANNELS[gs_adc_channel_idx]);
    ADCA.CH0.CTRL |= ADC_CH_START_bm;

    return true;
}

void adc_scan(void)
{
    uint8_t n = 0;
    while (n < N_ADC_CHANNELS) {
        bool got_result = adc_cycle();
        if (got_result) {
            ++n;
        }
    }
}

uint8_t adc_n_ch(void)
{
    return N_ADC_CHANNELS;
}

uint32_t adc_sample_n(uint8_t channel, uint16_t times)
{
    uint32_t sum = 0;
    for (uint16_t i = 0; i < times; ++i) {
        adc_scan();
        sum += get_adc_result(channel);
    }
    return sum;
}

uint16_t get_adc_result(uint8_t n)
{
    uint16_t result = 0;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        result = gs_adc_results[n];
    }
    return result;
}

uint8_t read_prodsig(uint8_t idx)
{
    uint8_t result;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        NVM_CMD = NVM_CMD_READ_CALIB_ROW_gc;
        result = pgm_read_byte(idx);
        NVM_CMD = NVM_CMD_NO_OPERATION_gc;
    }
    return result;
}

void crc_init(void)
{
    CRC.CTRL = CRC_RESET_RESET1_gc;
    CRC.CTRL = CRC_SOURCE_IO_gc;
}

void crc_process_byte(uint8_t byte)
{
    CRC.DATAIN = byte;
}

void crc_process_bytes(const uint8_t* bytes, size_t n)
{
    for (size_t i = 0; i < n; ++i) {
        crc_process_byte(bytes[i]);
    }
}

uint16_t crc_get_checksum(void)
{
    CRC.STATUS |= CRC_BUSY_bm;
    while (CRC.STATUS & CRC_BUSY_bm);
    uint16_t checksum = ((uint16_t) CRC.CHECKSUM0) & 0xff;
    checksum |= ((uint16_t) CRC.CHECKSUM1 << 8) & 0xff00;
    return checksum;
}

bool crc_is_checksum_zero(void)
{
    CRC.STATUS |= CRC_BUSY_bm;
    while (CRC.STATUS & CRC_BUSY_bm);
    return CRC.STATUS & CRC_ZERO_bm;
}
