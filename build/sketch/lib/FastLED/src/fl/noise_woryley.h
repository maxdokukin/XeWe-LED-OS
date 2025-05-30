#line 1 "/Users/xewe/Documents/Programming/Arduino/XeWe-LedOS/lib/FastLED/src/fl/noise_woryley.h"
#pragma once

#include <stdint.h>

namespace fl {

// Compute 2D Worley noise at (x, y) in Q15
int32_t worley_noise_2d_q15(int32_t x, int32_t y);

} // namespace fl
