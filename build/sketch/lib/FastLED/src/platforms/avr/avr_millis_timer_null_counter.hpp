#line 1 "/Users/xewe/Documents/Programming/Arduino/XeWe-LedOS/lib/FastLED/src/platforms/avr/avr_millis_timer_null_counter.hpp"
#pragma once

// Defines a weak symbol for timer_millis to allow linking with some Attiny boards
// This is a weak definition of timer_millis.
// It allows the code to compile and link, but it's expected that
// the actual implementation will be provided elsewhere (e.g., by the Arduino core).
__attribute__((weak)) volatile unsigned long timer_millis = 0;
