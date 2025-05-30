#line 1 "/Users/xewe/Documents/Programming/Arduino/XeWe-LedOS/lib/FastLED/src/fl/virtual_if_not_avr.h"
#pragma once

#ifdef __AVR__
#define VIRTUAL_IF_NOT_AVR
#else
#define VIRTUAL_IF_NOT_AVR virtual
#endif