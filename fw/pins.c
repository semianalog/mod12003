#include "pins.h"

#define X(port, pin, name, dir, dval, drive) {&PORT##port, (1 << pin)},
const struct PINS_pin_t PINS_pins[] =
{
    SYSTEM_CONF_PINS
};
#undef X
