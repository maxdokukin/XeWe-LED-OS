#pragma once

#include "../Mode/Mode.h"
#include <memory>

class ModeSolid : public Mode {
public:
    ModeSolid(uint16_t num_leds, const std::array<uint8_t, 3>& rgb);

    const CRGB* loop() override;
};

std::unique_ptr<Mode> make_mode_solid(uint16_t num_leds, const std::array<uint8_t, 3>& rgb);