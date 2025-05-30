#line 1 "/Users/xewe/Documents/Programming/Arduino/XeWe-LedOS/lib/FastLED/src/platforms/wasm/js_bindings.h"
#pragma once

#include "fl/stdint.h"

namespace fl {

class ScreenMap;
class ActiveStripData;

void jsSetCanvasSize(int cledcontoller_id, const fl::ScreenMap &screenmap);
void jsOnFrame(ActiveStripData &active_strips);
void jsOnStripAdded(uintptr_t strip, uint32_t num_leds);
void updateJs(const char *jsonStr);

} // namespace fl
