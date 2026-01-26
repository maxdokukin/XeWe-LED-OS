// LedModeController.h
#pragma once

#include <array>
#include <FastLED.h>

class LedModeController {
public:
  // HSV {h,s,v} -> RGB {r,g,b} using FastLED rainbow mapping
  static std::array<uint8_t, 3> hsv_to_rgb(uint8_t h, uint8_t s, uint8_t v);

  // RGB {r,g,b} -> HSV {h,s,v} (FastLED approx)
  static std::array<uint8_t, 3> rgb_to_hsv(uint8_t r, uint8_t g, uint8_t b);
};
