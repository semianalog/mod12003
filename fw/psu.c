#include "config.h"
#include "psu.h"
#include "hardware.h"

void psu_control_step(void)
{
    i2cdac_set(0);
}
