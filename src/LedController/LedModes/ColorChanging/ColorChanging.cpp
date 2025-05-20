#include "ColorChanging.h"

ColorChanging::ColorChanging(LedController* controller, uint8_t current_r, uint8_t current_g, uint8_t current_b, uint8_t target_r, uint8_t target_g, uint8_t target_b, uint32_t time)
    : LedMode(controller) {
    uint8_t current_rgb[3] = {current_r, current_g, current_b};
    uint8_t target_rgb[3] = {target_r, target_g, target_b};
    timer = new AsyncTimerArray(time, current_rgb, target_rgb);
    timer->initiate();
}

void ColorChanging::frame() {
    if (timer->is_not_initiated()) {
        return;
    }

    if (timer->is_done()) {
        timer->terminate();
    }

    std::array<uint8_t, 3> current_color = timer->get_current_value();
    led_controller->fill_all(current_color[0], current_color[1], current_color[2]);
    LedMode::rgb[0] = current_color[0];
    LedMode::rgb[1] = current_color[1];
    LedMode::rgb[2] = current_color[2];
}

bool ColorChanging::is_done() {
    return timer->is_done();
}

uint8_t ColorChanging::get_mode_id() {
    return 2;
}
