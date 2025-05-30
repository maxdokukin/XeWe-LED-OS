#line 1 "/Users/xewe/Documents/Programming/Arduino/XeWe-LedOS/lib/FastLED/src/platforms/wasm/js_assert.h"
#pragma once

#include "fl/warn.h"
#include <emscripten.h>

#define FASTLED_ASSERT(x, MSG)                                                 \
    {                                                                          \
        if (!(x)) {                                                            \
            FASTLED_WARN(MSG);                                                 \
            emscripten_debugger();                                             \
        }                                                                      \
    }