#include "Brightness.h"

Brightness::Brightness(LedController* controller, uint8_t initial_brightness, uint16_t transition_delay)
    : led_controller(controller) {
    timer = new AsyncTimer<uint8_t>(transition_delay, 0, initial_brightness);
    timer->initiate();
}

void Brightness::frame() {
    if (timer->is_not_initiated()) {
        return;
    }

    if (timer->is_done()) {
        timer->terminate();
    }

    timer->calculate_progress();
}

uint8_t Brightness::get_current_value() const {
    return timer->get_current_value();
}

uint8_t Brightness::get_target_value() const {
    return timer->get_target_value();
}

void Brightness::set_brightness(uint8_t new_brightness) {
    if (timer->is_active()) {
        return;
    }

    timer->reset(timer->get_current_value(), new_brightness);
    timer->initiate();
}

bool Brightness::is_changing() {
    return timer->is_active();
}
