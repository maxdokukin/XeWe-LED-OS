/*********************************************************************************
 *  SPDX-License-Identifier: LicenseRef-PolyForm-NC-1.0.0-NoAI
 *
 *  Licensed under PolyForm Noncommercial 1.0.0 + No AI Use Addendum v1.0.
 *  See: LICENSE and LICENSE-NO-AI.md in the project root for full terms.
 *
 *  Required Notice: Copyright 2025 Maxim Dokukin (https://maxdokukin.com)
 *  https://github.com/maxdokukin/xewe-led-os
 *********************************************************************************/
// src/Interfaces/Hardware/LedStrip/LedStrip.cpp

#include "LedStrip.h"

#include "../../../SystemController/SystemController.h"

// =============================================================================
// Constructor
// =============================================================================
LedStrip::LedStrip(SystemController& controller)
      : Interface(controller,
               /* interface_name        */  "Led",
               /* interface_description */  "Allows to control addressable LED strip",
               /* nvs_key               */  "led",
               /* requires_init_setup   */  true,
               /* can_be_disabled       */  false,
               /* has_cli_cmds          */  true)
{
    commands_storage.push_back({
        "set_rgb",
        "Set RGB color",
        string("$") + lower(module_name) + " set_rgb 255 0 0",
        3,
        [this](string_view args_sv) {
            String args(args_sv.data(), args_sv.length());
            int i1 = args.indexOf(' ');
            if (i1 == -1) return;
            int i2 = args.indexOf(' ', i1 + 1);
            if (i2 == -1) return;

            uint8_t r = args.substring(0, i1).toInt();
            uint8_t g = args.substring(i1 + 1, i2).toInt();
            uint8_t b = args.substring(i2 + 1).toInt();

            controller.sync_color({r, g, b}, {true, true, true, true, true});
        }
    });

    commands_storage.push_back({
        "set_r",
        "Set red channel",
        string("$") + lower(module_name) + " set_r 127",
        1,
        [this](string_view args_sv) {
            String args(args_sv.data(), args_sv.length());
            controller.sync_color({(uint8_t)args.toInt(), get_g(), get_b()}, {true, true, true, true, true});
        }
    });

    commands_storage.push_back({
        "set_g",
        "Set green channel",
        string("$") + lower(module_name) + " set_g 255",
        1,
        [this](string_view args_sv) {
            String args(args_sv.data(), args_sv.length());
            controller.sync_color({get_r(), (uint8_t)args.toInt(), get_b()}, {true, true, true, true, true});
        }
    });

    commands_storage.push_back({
        "set_b",
        "Set blue channel",
        string("$") + lower(module_name) + " set_b 200",
        1,
        [this](string_view args_sv) {
            String args(args_sv.data(), args_sv.length());
            controller.sync_color({get_r(), get_g(), (uint8_t)args.toInt()}, {true, true, true, true, true});
        }
    });

    commands_storage.push_back({
        "set_hsv",
        "Set HSV color",
        string("$") + lower(module_name) + " set_hsv 75 255 0",
        3,
        [this](string_view args_sv) {
            String args(args_sv.data(), args_sv.length());
            int i1 = args.indexOf(' ');
            if (i1 == -1) return;
            int i2 = args.indexOf(' ', i1 + 1);
            if (i2 == -1) return;

            uint8_t h = args.substring(0, i1).toInt();
            uint8_t s = args.substring(i1 + 1, i2).toInt();
            uint8_t v = args.substring(i2 + 1).toInt();

            array<uint8_t, 3> new_rgb = LedMode::hsv_to_rgb({h, s, v});
            controller.sync_color(new_rgb, {true, true, true, true, true});
        }
    });

    commands_storage.push_back({
        "set_hue",
        "Set hue channel",
        string("$") + lower(module_name) + " set_hue 255",
        1,
        [this](string_view args_sv) {
            String args(args_sv.data(), args_sv.length());
            array<uint8_t, 3> current_hsv = get_hsv();
            array<uint8_t, 3> new_rgb = LedMode::hsv_to_rgb({(uint8_t)args.toInt(), current_hsv[1], current_hsv[2]});
            controller.sync_color(new_rgb, {true, true, true, true, true});
        }
    });

    commands_storage.push_back({
        "set_sat",
        "Set saturation channel",
        string("$") + lower(module_name) + " set_sat 0",
        1,
        [this](string_view args_sv) {
            String args(args_sv.data(), args_sv.length());
            array<uint8_t, 3> current_hsv = get_hsv();
            array<uint8_t, 3> new_rgb = LedMode::hsv_to_rgb({current_hsv[0], (uint8_t)args.toInt(), current_hsv[2]});
            controller.sync_color(new_rgb, {true, true, true, true, true});
        }
    });

    commands_storage.push_back({
        "set_val",
        "Set value channel",
        string("$") + lower(module_name) + " set_val 255",
        1,
        [this](string_view args_sv) {
            String args(args_sv.data(), args_sv.length());
            array<uint8_t, 3> current_hsv = get_hsv();
            array<uint8_t, 3> new_rgb = LedMode::hsv_to_rgb({current_hsv[0], current_hsv[1], (uint8_t)args.toInt()});
            controller.sync_color(new_rgb, {true, true, true, true, true});
        }
    });

    commands_storage.push_back({
        "set_brightness",
        "Set global brightness",
        string("$") + lower(module_name) + " set_brightness 255",
        1,
        [this](string_view args_sv) {
            String args(args_sv.data(), args_sv.length());
            controller.sync_brightness(args.toInt(), {true, true, true, true, true});
        }
    });

    commands_storage.push_back({
        "set_state",
        "Set on/off state",
        string("$") + lower(module_name) + " set_state 0",
        1,
        [this](string_view args_sv) {
            String args(args_sv.data(), args_sv.length());
            controller.sync_state(args.toInt(), {true, true, true, true, true});
        }
    });

    commands_storage.push_back({
        "toggle_state",
        "If off->on, if on->off",
        string("$") + lower(module_name) + " toggle_state",
        0,
        [this](string_view) {
            controller.sync_state(!get_state(), {true, true, true, true, true});
        }
    });

    commands_storage.push_back({
        "turn_on",
        "Turn strip on",
        string("$") + lower(module_name) + " turn_on",
        0,
        [this](string_view) {
            controller.sync_state(1, {true, true, true, true, true});
        }
    });

    commands_storage.push_back({
        "turn_off",
        "Turn strip off",
        string("$") + lower(module_name) + " turn_off",
        0,
        [this](string_view) {
            controller.sync_state(0, {true, true, true, true, true});
        }
    });

    commands_storage.push_back({
        "set_mode",
        "Set LED strip mode",
        string("$") + lower(module_name) + " set_mode 0",
        1,
        [this](string_view args_sv) {
            String args(args_sv.data(), args_sv.length());
            controller.sync_mode(args.toInt(), {true, true, true, true, true});
        }
    });

    commands_storage.push_back({
        "set_length",
        "Set new number of LEDs",
        string("$") + lower(module_name) + " set_length 500",
        1,
        [this](string_view args_sv) {
            String args(args_sv.data(), args_sv.length());
            controller.sync_length(args.toInt(), {true, true, true, true, true});
        }
    });

    DBG_PRINTLN(LedStrip, "<- LedStrip::LedStrip()");
}

// =============================================================================
// Interface Sync
// =============================================================================
void LedStrip::sync_color(array<uint8_t,3> color) { set_rgb(color); }

void LedStrip::sync_brightness(uint8_t brightness) { set_brightness(brightness); }

void LedStrip::sync_state(uint8_t state) { set_state(state); }

void LedStrip::sync_mode(uint8_t mode) { set_mode(mode); }

void LedStrip::sync_length(uint16_t length) { set_length(length); }

// =============================================================================
// Module Logic
// =============================================================================
void LedStrip::begin_routines_required(const ModuleConfig& cfg) {
    const auto& config = static_cast<const LedStripConfig&>(cfg);
    this->num_led                = config.num_led;
    this->mode_transition_delay  = config.mode_transition_delay;

    FastLED.addLeds<LED_STRIP_TYPE, PIN_LED_STRIP, LED_STRIP_COLOR_ORDER>(leds, LED_STRIP_NUM_LEDS_MAX).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(255);

    frame_timer = make_unique<AsyncTimer<uint8_t>>(config.led_controller_frame_delay);
    brightness = make_unique<Brightness>(config.brightness_transition_delay, 0, 0);
    mode_controller = make_unique<ModeController>(this);

    frame_timer->initiate();
}

void LedStrip::begin_routines_init(const ModuleConfig& cfg) {
    this->num_led = controller.serial_port.get_int("How many LEDs do you have connected", 0, LED_STRIP_NUM_LEDS_MAX + 1);
    controller.sync_all(
        {0, 255,  0},
        50,
        1,
        0,
        this->num_led,
        {true, true, false, false, false} //only write to nvs and led
    );
    controller.serial_port.print("\nLED strip is set to green\n"
                                   "If you don't see the green color check the\n"
                                   "pin (GPIO), led type, and color order\n\n"
                                   "LED setup success!");
}

void LedStrip::begin_routines_regular(const ModuleConfig& cfg) {

    controller.nvs.sync_from_memory({true, false, false, false, false});
}

void LedStrip::begin_routines_common(const ModuleConfig& cfg) {
    controller.serial_port.print("Setting up LED lights");
    run_with_dots([this] { loop(); }, (float) color_transition_delay * 1.2f);
}

void LedStrip::loop() {
    if (frame_timer->is_not_done()) return;
    frame_timer->reset();
    frame_timer->initiate();

    mode_controller->loop();
    fill_all();
}

void LedStrip::reset(const bool verbose, const bool do_restart, const bool keep_enabled) {
    controller.sync_all(
        {0, 255,  0},
        50,
        1,
        0,
        this->num_led,
        {true, true, true, true, true}
    );
    if (verbose) status(true);
    Module::reset(verbose, do_restart, keep_enabled);
}

string LedStrip::status(const bool verbose) const {
    std::stringstream status_stream;
    status_stream << "Hardware Setting:\n"
                  << "    Pin:          GPIO" << static_cast<int>(PIN_LED_STRIP) << "\n"
                  << "    Type:         " << TO_STRING(LED_STRIP_TYPE) << "\n"
                  << "    Color Order:  " << TO_STRING(LED_STRIP_COLOR_ORDER) << "\n"
                  << "    Max LEDs:     " << LED_STRIP_NUM_LEDS_MAX << "\n"
                  << "\n"
                  << "Live State:\n"
                  << "    FPS:          " << fps_counter * 1000 / millis()  << "\n"
                  << "    Length:       " << get_length() << "\n"
                  << "    State:        " << (get_state() ? "ON" : "OFF") << "\n"
                  << "    Brightness:   " << static_cast<int>(get_brightness()) << "\n"
                  << "    Mode:         " << get_mode_name().c_str() << "\n"
                  << "    Color (RGB):  ("
                  << static_cast<int>(get_r()) << ", "
                  << static_cast<int>(get_g()) << ", "
                  << static_cast<int>(get_b()) << ")";

    std::string status_string = status_stream.str();
    if (verbose) controller.serial_port.print(status_string.c_str());
    return status_string;
}

// =============================================================================
// Custom Methods: Color
// =============================================================================
void LedStrip::set_rgb(const array<uint8_t, 3> new_rgb) {

    mode_controller->set_rgb(new_rgb);
}

void LedStrip::set_r(const uint8_t r) {
    array<uint8_t, 3> old_rgb = get_rgb();
    mode_controller->set_rgb({r, old_rgb[1], old_rgb[2]});
}

void LedStrip::set_g(const uint8_t g) {
    array<uint8_t, 3> old_rgb = get_rgb();
    mode_controller->set_rgb({old_rgb[0], g, old_rgb[2]});
}

void LedStrip::set_b(const uint8_t b) {
    array<uint8_t, 3> old_rgb = get_rgb();
    mode_controller->set_rgb({old_rgb[0], old_rgb[1], b});
}

void LedStrip::set_hsv(const array<uint8_t, 3> new_hsv) {
    array<uint8_t, 3> new_rgb = ModeController::hsv_to_rgb(new_hsv[0], new_hsv[1], new_hsv[2]);
    mode_controller->set_rgb(new_rgb);
}

void LedStrip::set_h(const uint8_t h) {
    array<uint8_t, 3> old_hsv = get_hsv();
    mode_controller->set_rgb(ModeController::hsv_to_rgb(h, old_hsv[1], old_hsv[2]));
}

void LedStrip::set_s(const uint8_t s) {
    array<uint8_t, 3> old_hsv = get_hsv();
    mode_controller->set_rgb(ModeController::hsv_to_rgb(old_hsv[0], s, old_hsv[2]));
}

void LedStrip::set_v(const uint8_t v) {
    array<uint8_t, 3> old_hsv = get_hsv();
    mode_controller->set_rgb(ModeController::hsv_to_rgb(old_hsv[0], old_hsv[1], v));
}

array<uint8_t, 3> LedStrip::get_rgb() const {

    return mode_controller->get_rgb();
}

uint8_t LedStrip::get_r() const {

    return get_rgb()[0];
}

uint8_t LedStrip::get_g() const {

    return get_rgb()[1];
}

uint8_t LedStrip::get_b() const {

    return get_rgb()[2];
}

array<uint8_t, 3> LedStrip::get_hsv() const {
    array<uint8_t, 3> rgb = get_rgb();
    return ModeController::rgb_to_hsv(rgb[0], rgb[1], rgb[2]);
}

uint8_t LedStrip::get_h() const {

    return get_hsv()[0];
}

uint8_t LedStrip::get_s() const {

    return get_hsv()[1];
}

uint8_t LedStrip::get_v() const {

    return get_hsv()[2];
}

// =============================================================================
// Custom Methods: Brightness
// =============================================================================
void LedStrip::set_brightness(const uint8_t new_brightness) {

    brightness->set_brightness(new_brightness);
}

uint8_t LedStrip::get_brightness() const {

    return brightness->get_last_brightness();
}

// =============================================================================
// Custom Methods: State
// =============================================================================
void LedStrip::set_state(const uint8_t state) {
    if (state_val) {
        turn_on();
    } else {
        turn_off();
    }
}

void LedStrip::toggle_state() {
    if(get_state()) {
        turn_off();
    } else {
        turn_on();
    }
}

void LedStrip::turn_on() {

    brightness->turn_on();
}

void LedStrip::turn_off() {

    brightness->turn_off();
}

bool LedStrip::get_state() const {

    return brightness->get_state();
}

// =============================================================================
// Custom Methods: Mode
// =============================================================================
void LedStrip::set_mode(const uint8_t new_mode) {

    mode_controller->set_mode(new_mode);
}

uint8_t LedStrip::get_mode() const {

    return mode_controller->get_mode();
}

string LedStrip::get_mode_name() const {

    return mode_controller->get_mode_name();
}

string LedStrip::get_all_modes() const {

    return mode_controller->get_all_modes();
}

// =============================================================================
// Custom Methods: Length
// =============================================================================
void LedStrip::set_length(const uint16_t length) {
    if (new_length > LED_STRIP_NUM_LEDS_MAX) {
        controller.serial_port.print("That's too many. Max supported: " + to_string(LED_STRIP_NUM_LEDS_MAX) + " LEDs");
        return;
    }

    set_all(0, 0, 0);
    FastLED.show();
    num_led = new_length;
}

uint16_t LedStrip::get_length() const {

    return num_led;
}
