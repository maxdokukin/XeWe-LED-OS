#line 1 "/Users/xewe/Documents/Programming/Arduino/XeWe-LedOS/lib/FastLED/src/fl/types.h"
#pragma once

#include "fl/namespace.h"
#include "led_sysdefs.h"

namespace fl {

#if defined(__AVR__)
typedef int cycle_t; ///< 8.8 fixed point (signed) value
#else
typedef int64_t cycle_t; ///< 8.8 fixed point (signed) value
#endif

} // namespace fl
