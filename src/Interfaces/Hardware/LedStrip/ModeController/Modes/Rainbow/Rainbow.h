#pragma once

#include "../Mode/Mode.h"
#include <memory>

class Rainbow : public Mode {
public:
    Rainbow(uint16_t num_leds, const std::array<uint8_t, 3>& rgb);

    const CRGB* loop() override;

private:
    uint8_t _hue; // Internal state to animate the rainbow movement
};

// Factory declaration
std::unique_ptr<Mode> make_mode_rainbow(uint16_t num_leds, const std::array<uint8_t, 3>& rgb);