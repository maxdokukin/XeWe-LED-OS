#include "Fade.h"

Fade::Fade(uint16_t num_leds, const std::array<uint8_t, 3>& rgb)
    : Mode(num_leds, 1, "Fade", rgb)
{}

const CRGB* Fade::loop() {
    // Create the base color from stored RGB
    CRGB color = CRGB(_rgb[0], _rgb[1], _rgb[2]);

    // Calculate brightness using a sine wave (approx 30 beats per minute)
    // beatsin8 returns a value between 0-255
    uint8_t brightness = beatsin8(30);

    // Apply brightness to the color
    color.nscale8(brightness);

    // Fill the buffer
    fill_solid(_leds.data(), _num_leds, color);

    return _leds.data();
}

// Factory Implementation
std::unique_ptr<Mode> make_mode_fade(uint16_t num_leds, const std::array<uint8_t, 3>& rgb) {
    return std::make_unique<Fade>(num_leds, rgb);
}