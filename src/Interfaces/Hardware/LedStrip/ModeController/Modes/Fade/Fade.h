#pragma once

#include "../Mode/Mode.h"
#include <memory>

class Fade : public Mode {
public:
    Fade(uint16_t num_leds, const std::array<uint8_t, 3>& rgb);

    const CRGB* loop() override;

private:
    // Helper to replicate the 16-bit math from your original algorithm
    CRGB get_fire_color(uint8_t noise_val, long base_hue_16bit);

    // Wrapper to handle 16-bit hue -> 8-bit FastLED conversion
    CRGB ColorHSV(long hue, uint8_t sat, uint8_t val);

    // Animation state
    uint32_t counter;

    // --- Hardcoded Configuration (Original Values) ---
    // 10000 is ~15% of the 16-bit color wheel (65535)
    static const int HUE_GAP    = 10000;
    static const int FIRE_STEP  = 10;
    static const int MIN_BRIGHT = 100;
    static const int MAX_BRIGHT = 255;
    static const int MIN_SAT    = 245;
    static const int MAX_SAT    = 255;

    // Derived constant for centering the gap
    static const int HALF_HUE_GAP = HUE_GAP / 2;
};

// Factory declaration
std::unique_ptr<Mode> make_mode_fade(uint16_t num_leds, const std::array<uint8_t, 3>& rgb);