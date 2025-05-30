#line 1 "/Users/xewe/Documents/Programming/Arduino/XeWe-LedOS/lib/FastLED/src/fl/force_inline.h"
#pragma once

#ifdef FASTLED_NO_FORCE_INLINE
#define FASTLED_FORCE_INLINE inline
#else
#define FASTLED_FORCE_INLINE __attribute__((always_inline)) inline
#endif
