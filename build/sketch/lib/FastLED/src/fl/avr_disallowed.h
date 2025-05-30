#line 1 "/Users/xewe/Documents/Programming/Arduino/XeWe-LedOS/lib/FastLED/src/fl/avr_disallowed.h"
#pragma once

#ifdef __AVR__
#define AVR_DISALLOWED                                                         \
    [[deprecated("This function or class is deprecated on AVR.")]]
#else
#define AVR_DISALLOWED
#endif
