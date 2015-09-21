#ifndef _PINS_H
#define _PINS_H 1

#include "config.h"

/* Code to set or toggle pins, using the pin definitions in config.h */
/* This is not incredibly efficient, and shouldn't be used for fast
 * bit-banging. */


/* Create enum linking pin names to IDs */
#define X(port, pin, name, dir, dval, drive) name,
enum PINS_name_to_id {
SYSTEM_CONF_PINS
};
#undef X

struct PINS_pin_t
{
    PORT_t *portname;
    unsigned char pinmask; /* Stored as a mask to save a step */
};

/* External array holding pins and ports */
extern const struct PINS_pin_t PINS_pins[];

/* Set, clear, toggle */
#define PIN_SET(pin) PINS_pins[(pin)].portname->OUT |= PINS_pins[(pin)].pinmask
#define PIN_CLR(pin) PINS_pins[(pin)].portname->OUT &= ~(PINS_pins[(pin)].pinmask)
#define PIN_TGL(pin) PINS_pins[(pin)].portname->OUT ^= PINS_pins[(pin)].pinmask

/* Read - returns the bit, not 1 or 0 */
#define PIN_GET(pin) (PINS_pins[(pin)].portname->IN & PINS_pins[(pin)].pinmask)

#endif /* _PINS_H */
