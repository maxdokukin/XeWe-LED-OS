// LedModeController.cpp
#include "LedModeController.h"
#include "Mode.h"
#include "AsyncTimer.h"

unique_ptr<Mode> make_mode_solid();
unique_ptr<Mode> make_mode_fade();
unique_ptr<Mode> make_mode_rainbow();

ModeController::ModeController(uint16_t mode_transition_delay) {
    transition_timer = std::make_unique<AsyncTimer<uint16_t>>(mode_transition_delay);
    transition_timer->activate();
    
    register_modes();
    begin_transition(mode_registry[0].make(), mode_registry[0].id);    
}

void ModeController::register_modes() {
    mode_registry = {{
        {0, "Solid",   &make_mode_solid},
        {1, "Fade", &make_mode_fade},
        {2, "Rainbow",   &make_mode_rainbow}
    }};
}

const ModeController::ModeDesc* ModeController::find_mode(uint8_t id) const {
    for (const auto& m : mode_registry)
        if (m.id == id)
            return &m;

    return nullptr;
}

void ModeController::begin_transition(unique_ptr<Mode> next, uint8_t next_id) {
    old_mode = std::move(current_mode);
    new_mode = std::move(next);

    transition_timer->reset();
    transition_timer->activate();
}

CRGB* ModeController::loop() {
    if (transition_timer->is_active()) {
        const CRGB* a = old_mode->loop();
        const CRGB* b = new_mode->loop();

        uint8_t amount = (uint8_t)(transition_timer->get_progress() * 255.0f + 0.5f);

        for (size_t i = 0; i < led_strip->get_length(); ++i)
            frame[i] = blend(a[i], b[i], amt);

        if (transition_timer->is_done()) {
            current_mode = std::move(new_mode);
            new_mode = null;
            old_mode = null;
        }
        return frame;
    }

    return current_mode->loop();
}

void ModeController::set_rgb(const array<uint8_t, 3> new_rgb) {
    const ModeDesc* d = find_mode(current_mode->get_id());

    old_mode = std::move(current_mode);
    current_mode = std::move(d->make(new_rgb));

    transition_timer->reset();
    transition_timer->initiate();
}

array<uint8_t, 3> ModeController::get_rgb() const {
    return current_mode->get_rgb();
}

void ModeController::set_mode(const uint8_t new_mode) {
    const ModeDesc* d = find_mode(new_mode);

    old_mode = std::move(current_mode);
    current_mode = std::move(d->make(old_mode->get_rgb()));

    transition_timer->reset();
    transition_timer->initiate();
}

uint8_t ModeController::get_mode() const {

    return current_mode->get_id();
}

string ModeController::get_mode_name() const {
    return current_mode->get_name();
}

string ModeController::get_all_modes() const {
    string s = "[";
    for (size_t i = 0; i < mode_registry.size(); ++i) {
        if (i) s += ",";
        s += "{\"id\":";
        s += std::to_string(mode_registry[i].id);
        s += ",\"name\":\"";
        s += mode_registry[i].name ? mode_registry[i].name : "";
        s += "\"}";
    }
    s += "]";
    return s;
}

std::array<uint8_t, 3> ModeController::hsv_to_rgb(uint8_t h, uint8_t s, uint8_t v) {
    CRGB rgb;
    hsv2rgb_rainbow(CHSV(h, s, v), rgb);
    return {rgb.r, rgb.g, rgb.b};
}

std::array<uint8_t, 3> ModeController::rgb_to_hsv(uint8_t r, uint8_t g, uint8_t b) {
    CHSV hsv = rgb2hsv_approximate(CRGB(r, g, b));
    return {hsv.h, hsv.s, hsv.v};
}
