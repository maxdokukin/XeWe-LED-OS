#include "Rainbow.h"

// Note: Rainbow generally ignores the input RGB, but we keep the signature consistent
Rainbow::Rainbow(uint16_t num_leds, const std::array<uint8_t, 3>& rgb)
    : Mode(num_leds, 2, "Rainbow", rgb), _hue(0)
{}

const CRGB* Rainbow::loop() {
    // fill_rainbow(leds, num_leds, initial_hue, delta_hue)
    fill_rainbow(_leds.data(), _num_leds, _hue, 7);
    
    // Increment hue to animate movement for the next frame
    _hue++; 

    return _leds.data();
}

// Factory Implementation
std::unique_ptr<Mode> make_mode_rainbow(uint16_t num_leds, const std::array<uint8_t, 3>& rgb) {
    return std::make_unique<Rainbow>(num_leds, rgb);
}