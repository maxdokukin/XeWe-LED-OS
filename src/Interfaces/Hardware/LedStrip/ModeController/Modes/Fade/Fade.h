#pragma once

#include "../Mode/Mode.h"
#include <memory>

class Fade : public Mode {
public:
    Fade(uint16_t num_leds, const std::array<uint8_t, 3>& rgb);

    const CRGB* loop() override;
};

// Factory declaration
std::unique_ptr<Mode> make_mode_fade(uint16_t num_leds, const std::array<uint8_t, 3>& rgb);