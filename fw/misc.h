#ifndef __MISC_H
#define __MISC_H 1


/**
 * Macro to define a static function and disable unused-function warnings.
 */
#define UNUSED(f) f __attribute__((unused)); f

#endif // __MISC_H
