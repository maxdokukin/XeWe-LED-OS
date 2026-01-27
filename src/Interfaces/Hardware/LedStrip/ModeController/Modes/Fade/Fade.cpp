#include "Fade.h"

Fade::Fade(uint16_t num_leds, const std::array<uint8_t, 3>& rgb)
    : Mode(num_leds, 1, "Fade", rgb), counter(0)
{}

const CRGB* Fade::loop() {
    // 1. Get the current Base Hue
    CRGB current_rgb = CRGB(_rgb[0], _rgb[1], _rgb[2]);
    CHSV base_hsv_8bit = rgb2hsv_approximate(current_rgb);

    // Scale 8-bit hue (0-255) up to 16-bit (0-65535) to match the algorithm's expected range
    long base_hue_16bit = (long)base_hsv_8bit.hue * 256;

    // 2. Frame Update
    for (int i = 0; i < _num_leds; i++) {
        // Calculate noise (Standard FastLED 8-bit noise)
        uint8_t noise = inoise8(i * FIRE_STEP, counter);

        // Perform the calculation in high precision
        _leds[i] = get_fire_color(noise, base_hue_16bit);
    }

    // 3. Increment counter (Speed)
    counter += 5;

    return _leds.data();
}

CRGB Fade::get_fire_color(uint8_t val, long base_hue_16bit) {
    // --- EXACT MATH PORT ---

    // Original: timer->get_current_value() - adjusted_hue_gap_half + map(val, 0, 255, 0, adjusted_hue_gap)
    // We use long to keep the 10,000 range precision
    long calculated_hue = base_hue_16bit - HALF_HUE_GAP + map(val, 0, 255, 0, HUE_GAP);

    // Map Saturation and Brightness
    uint8_t calculated_sat = constrain(map(val, 0, 255, MAX_SAT, MIN_SAT), 0, 255);
    uint8_t calculated_val = constrain(map(val, 0, 255, MIN_BRIGHT, MAX_BRIGHT), 0, 255);

    return ColorHSV(calculated_hue, calculated_sat, calculated_val);
}

CRGB Fade::ColorHSV(long hue, uint8_t sat, uint8_t val) {
    // The hue coming in is 16-bit (e.g., 35000)
    // FastLED expects 8-bit (e.g., 136)
    // We bit-shift right by 8 (equivalent to dividing by 256) at the LAST moment.
    // This preserves the smooth gradient logic of the calculation above.
    return CHSV((uint16_t)hue >> 8, sat, val);
}

// Factory Implementation
std::unique_ptr<Mode> make_mode_fade(uint16_t num_leds, const std::array<uint8_t, 3>& rgb) {
    return std::make_unique<Fade>(num_leds, rgb);
}