#line 1 "/Users/xewe/Documents/Programming/Arduino/XeWe-LedOS/lib/FastLED/src/platforms/esp/esp_assert.h"
#pragma once

#include "esp_log.h"

#define FASTLED_ASSERT(x, MSG)                                                 \
    {                                                                          \
        if (!(x)) {                                                            \
            ESP_LOGE("#### FastLED", "%s", (fl::StrStream() << MSG).c_str());  \
        }                                                                      \
    }
