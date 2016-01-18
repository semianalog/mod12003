#include <afw/pins.h>
#include <util/delay.h>
#include <util/atomic.h>
#include "psu.h"
#include "measure.h"
#include "hardware.h"
#include "cal.h"
#include "timers.h"
#include "analog.h"
#include "misc_math.h"

static int16_t          gs_voltage_setpoint         = 0;
static int16_t          gs_current_setpoint         = 0;

static int32_t          gs_correction_mv            = 0;
static volatile bool    gs_enabled                  = false;

void psu_enable(bool enabled)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        gs_enabled = enabled;
    }
}

bool psu_enabled(void)
{
    return PGET(P_LINREG_EN);
}

enum psu_reg_mode psu_get_reg_mode(void)
{
    bool cv = PGET(P_VLIM);
    bool cc = PGET(P_ILIM);

    if ((cc && cv) || (!cc && !cv)) {
        return PSU_OSCILLATING;
    } else if (cc) {
        return PSU_REG_CC;
    } else {
        return PSU_REG_CV;
    }
}

void psu_vset(uint16_t mv)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        gs_voltage_setpoint = mv;
    }
}

void psu_iset(uint16_t ma)
{
    uint16_t word = linear(CAL_DATA_CURRENT.dacslope, ma, CAL_DATA_CURRENT.dacoffset);
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        gs_current_setpoint = ma;
        idac_set(word); // XXX - get this out of a potential ISR call
    }
}

uint16_t psu_get_vsetpt(void)
{
    return ATOMIC_ACCESS(gs_voltage_setpoint);
}

uint16_t psu_get_isetpt(void)
{
    return ATOMIC_ACCESS(gs_current_setpoint);
}

void psu_prereg_vset(uint16_t mv)
{
    // As before, PWM_TOP + 1 trades a bit of accuracy for efficiency.
    uint16_t dutycyc = (((uint32_t) PWM_TOP + 1) * mv) / INPUT_NOMINAL_MV;
    if (dutycyc > PWM_TOP) {
        dutycyc = PWM_TOP;
    }
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        timer_pwmset_prereg(dutycyc);
    }
}

void psu_fast_cycle(void)
{
    if (!gs_enabled) {
        return;
    }

    sense_filter_cycle();

    uint16_t prereg = measured_prereg();
    uint16_t postreg = last_measured_voltage();
    uint16_t prereg_margin = prereg - postreg;

    if (prereg < postreg || prereg_margin < REGULATOR_HEADROOM_LIMIT_MV) {
        psu_prereg_vset(last_measured_voltage() + REGULATOR_HEADROOM_LIMIT_MV);
    } else {
        psu_prereg_vset(gs_voltage_setpoint + REGULATOR_HEADROOM_LIMIT_MV);
    }
}

static void update_leds(void)
{
    if (gs_enabled) {
        enum psu_reg_mode mode = psu_get_reg_mode();
        switch (mode) {
        case PSU_REG_CV:
            PSET(P_LEDCV);
            PCLR(P_LEDCC);
            break;
        case PSU_REG_CC:
            PSET(P_LEDCC);
            PCLR(P_LEDCV);
            break;
        case PSU_OSCILLATING:
        default:
            PSET(P_LEDCC);
            PSET(P_LEDCV);
        }
    } else {
        PCLR(P_LEDCC);
        PCLR(P_LEDCV);
        return;
    }
}

/// Enable or disable on edges of gs_enabled
static void enable_or_disable(void)
{
    static bool s_last_enabled = false;

    if (gs_enabled && !s_last_enabled) {
        gs_correction_mv = 0;
        vdac_set(0);

        // Force the regulator to saturate low during the edge of
        // LINREG_EN, because otherwise it tends to saturate high
        // and cause output blips due to integrator windup.
        PSET(P_LINREG_SAT);
        _delay_us(50);
        PSET(P_LINREG_EN);
        _delay_us(50);
        PCLR(P_LINREG_SAT);

    } else if (!gs_enabled && s_last_enabled) {
        PCLR(P_LINREG_EN);
        psu_prereg_vset(0);
    }

    s_last_enabled = gs_enabled;
}

/// Perform one cycle of the voltage integral controller
static void voltage_integrator_cycle(void)
{
    static int32_t s_voltage_error_accum = 0;

    int16_t error = measured_voltage() - gs_voltage_setpoint;

    // If the single-step error is greater than the windup limit, don't just clamp,
    // completely discard the sample. This happens at voltage setpoint steps.
    if (error > VOLTAGE_WINDUP_LIMIT_RAW || error < -VOLTAGE_WINDUP_LIMIT_RAW) {
        error = 0;
    }

    s_voltage_error_accum += error;

    if (s_voltage_error_accum > VOLTAGE_WINDUP_LIMIT_RAW) {
        s_voltage_error_accum = VOLTAGE_WINDUP_LIMIT_RAW;
    } else if (s_voltage_error_accum < -VOLTAGE_WINDUP_LIMIT_RAW) {
        s_voltage_error_accum = -VOLTAGE_WINDUP_LIMIT_RAW;
    }

    gs_correction_mv = VOLTAGE_KI_NUMER * s_voltage_error_accum / VOLTAGE_KI_DENOM;
}

/// Update the DACs
static void psu_update(void)
{
    int16_t mv = gs_voltage_setpoint - gs_correction_mv;
    uint16_t word = linear(CAL_DATA_VOLTAGE.dacslope, mv, CAL_DATA_VOLTAGE.dacoffset);
    vdac_set(word);
}


void psu_slow_cycle(void)
{

    update_leds();
    enable_or_disable();

    if (gs_enabled) {
        voltage_integrator_cycle();
        psu_update();
    }
}

