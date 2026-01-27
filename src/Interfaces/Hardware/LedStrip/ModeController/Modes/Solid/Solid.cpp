#include "Solid.h"

Solid::Solid(uint16_t num_leds, const std::array<uint8_t, 3>& rgb)
    : Mode(num_leds, 0, "Solid", rgb)
{}

const CRGB* Solid::loop() {
    // 1. Create a FastLED CRGB color from the stored array
    CRGB color = CRGB(_rgb[0], _rgb[1], _rgb[2]);

    // 2. Fill the internal vector buffer with that color
    // We use _leds.data() to get the raw pointer from the vector
    fill_solid(_leds.data(), _num_leds, color);

    // 3. Return the pointer to the data for the controller to blend/display
    return _leds.data();
}

// Factory Implementation
std::unique_ptr<Mode> make_mode_solid(uint16_t num_leds, const std::array<uint8_t, 3>& rgb) {
    return std::make_unique<Solid>(num_leds, rgb);
}