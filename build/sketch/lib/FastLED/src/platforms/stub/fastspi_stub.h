#line 1 "/Users/xewe/Documents/Programming/Arduino/XeWe-LedOS/lib/FastLED/src/platforms/stub/fastspi_stub.h"
#pragma once


#ifdef __EMSCRIPTEN__
#include "platforms/wasm/fastspi_wasm.h"
#elif defined(FASTLED_STUB_IMPL)
#include "platforms/stub/fastspi_stub_generic.h"
#else
#error "This file should only be included for stub or emscripten builds"
#endif