#ifndef __MISC_H
#define __MISC_H 1


/**
 * Macro to define a static function and disable unused-function warnings.
 */
#define UNUSED(f) f __attribute__((unused)); f

/**
 * Combined 8 bit, 16 bit access
 */
union u16_u8 {
    uint16_t u16;
    uint8_t u8[2];
};

/// Access the low byte of a uint16_t
#define U16_LO(x) (((union u16_u8 *)(&(x)))->u8[0])

/// Access the high byte of a uint16_t
#define U16_HI(x) (((union u16_u8 *)(&(x)))->u8[1])


#endif // __MISC_H
