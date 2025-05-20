#ifndef BRIGHTNESS_H
#define BRIGHTNESS_H

#include <cstdint>

#include "../AsyncTimer/AsyncTimer.h"

class LedController;

class Brightness {
public:
    Brightness(LedController* controller, uint8_t initial_brightness, uint16_t transition_delay);

    void frame();
    void set_brightness(uint8_t brightness);

    bool is_changing();
    uint8_t get_current_value() const;
    uint8_t get_target_value() const;

private:
    LedController* led_controller;
    AsyncTimer<uint8_t>* timer;
};

#endif // BRIGHTNESS_H
