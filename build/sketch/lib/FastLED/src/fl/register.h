#line 1 "/Users/xewe/Documents/Programming/Arduino/XeWe-LedOS/lib/FastLED/src/fl/register.h"
#pragma once

/// @def FASTLED_REGISTER
/// Helper macro to replace the deprecated 'register' keyword if we're
/// using modern C++ where it's been removed entirely.

#if __cplusplus < 201703L
#define FASTLED_REGISTER register
#else
#ifdef FASTLED_REGISTER
#undef FASTLED_REGISTER
#endif
#define FASTLED_REGISTER
#endif