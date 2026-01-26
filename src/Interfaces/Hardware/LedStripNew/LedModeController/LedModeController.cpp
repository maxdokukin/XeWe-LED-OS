// LedModeController.cpp
#include "LedModeController.h"

std::array<uint8_t, 3> LedModeController::hsv_to_rgb(uint8_t h, uint8_t s, uint8_t v) {
  CRGB rgb;
  hsv2rgb_rainbow(CHSV(h, s, v), rgb);
  return { rgb.r, rgb.g, rgb.b };
}

std::array<uint8_t, 3> LedModeController::rgb_to_hsv(uint8_t r, uint8_t g, uint8_t b) {
  CHSV hsv = rgb2hsv_approximate(CRGB(r, g, b));
  return { hsv.h, hsv.s, hsv.v };
}
