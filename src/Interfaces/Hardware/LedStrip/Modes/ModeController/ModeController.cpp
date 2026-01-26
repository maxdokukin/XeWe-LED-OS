// LedModeController.cpp
#include "LedModeController.h"

ModeController::ModeController(uint16_t mode_transition_delay) {
    mode_transition_delay = mode_transition_delay;
}

void ModeController::loop() {

    current_mode->loop();
}

void ModeController::set_rgb(const array<uint8_t, 3> new_rgb) {

    current_mode->set_rgb();
}

array<uint8_t, 3> ModeController::get_rgb() const {

    return current_mode->get_rgb();
}

void ModeController::set_mode(const uint8_t new_mode) {

}

uint8_t ModeController::get_mode() const {

    return current_mode->get_id();
}

string ModeController::get_mode_name() const {

    return current_mode->get_name();
}

string ModeController::get_all_modes() const {

    return mode_map.as_json();
}

std::array<uint8_t, 3> LedModeController::hsv_to_rgb(uint8_t h, uint8_t s, uint8_t v) {
  CRGB rgb;
  hsv2rgb_rainbow(CHSV(h, s, v), rgb);
  return { rgb.r, rgb.g, rgb.b };
}

std::array<uint8_t, 3> LedModeController::rgb_to_hsv(uint8_t r, uint8_t g, uint8_t b) {
  CHSV hsv = rgb2hsv_approximate(CRGB(r, g, b));
  return { hsv.h, hsv.s, hsv.v };
}
