#include "LedModeController.h"
#include "Mode.h"

unique_ptr<Mode> make_mode_solid();
unique_ptr<Mode> make_mode_rainbow();
unique_ptr<Mode> make_mode_blink();

ModeController::ModeController(uint16_t mode_transition_delay)
: mode_transition_delay(mode_transition_delay),
  current_mode_id(0) {
    register_modes();
    set_mode(mode_registry[0].id);
}

void ModeController::register_modes() {
    mode_registry = {{
        {0, "Solid",   &make_mode_solid},
        {1, "Rainbow", &make_mode_rainbow},
        {2, "Blink",   &make_mode_blink}
    }};
}

const ModeController::ModeDesc* ModeController::find_mode(uint8_t id) const {
    for (const auto& m : mode_registry) if (m.id == id) return &m;
    return nullptr;
}

void ModeController::loop() {
    if (current_mode) current_mode->loop();
}

void ModeController::set_rgb(const array<uint8_t, 3> new_rgb) {
    if (current_mode) current_mode->set_rgb(new_rgb);
}

array<uint8_t, 3> ModeController::get_rgb() const {
    if (!current_mode) return {0, 0, 0};
    return current_mode->get_rgb();
}

void ModeController::set_mode(const uint8_t new_mode) {
    const ModeDesc* d = find_mode(new_mode);
    if (!d || !d->make) return;

    array<uint8_t, 3> rgb = get_rgb();
    unique_ptr<Mode> next = d->make();
    if (!next) return;

    current_mode = std::move(next);
    current_mode_id = d->id;
    current_mode->set_rgb(rgb);
}

uint8_t ModeController::get_mode() const {
    return current_mode_id;
}

string ModeController::get_mode_name() const {
    const ModeDesc* d = find_mode(current_mode_id);
    return d ? string(d->name) : string();
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
