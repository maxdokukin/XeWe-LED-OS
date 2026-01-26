/*********************************************************************************
 *  SPDX-License-Identifier: LicenseRef-PolyForm-NC-1.0.0-NoAI
 *
 *  Licensed under PolyForm Noncommercial 1.0.0 + No AI Use Addendum v1.0.
 *  See: LICENSE and LICENSE-NO-AI.md in the project root for full terms.
 *
 *  Required Notice: Copyright 2025 Maxim Dokukin (https://maxdokukin.com)
 *  https://github.com/maxdokukin/xewe-led-os
 *********************************************************************************/
// src/Interfaces/Hardware/LedStripNew/LedStripNew.cpp

#include "LedStripNew.h"
#include "../../../SystemController/SystemController.h"

// =============================================================================
// Constructor
// =============================================================================
LedStripNew::LedStripNew(SystemController& controller)
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
void LedStripNew::sync_color(array<uint8_t,3> color) { set_rgb(color); }

void LedStripNew::sync_brightness(uint8_t brightness) { set_brightness(brightness); }

void LedStripNew::sync_state(uint8_t state) { set_state(state); }

void LedStripNew::sync_mode(uint8_t mode) { set_mode(mode); }

void LedStripNew::sync_length(uint16_t length) { set_length(length); }


// =============================================================================
// Module Logic
// =============================================================================
void LedStripNew::begin_routines_required(const ModuleConfig& cfg) {
    const auto& config = static_cast<const LedStripConfig&>(cfg);
    this->num_led                = config.num_led;
    this->mode_transition_delay  = config.mode_transition_delay;

    FastLED.addLeds<LED_STRIP_TYPE, PIN_LED_STRIP, LED_STRIP_COLOR_ORDER>(leds, LED_STRIP_NUM_LEDS_MAX).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(255);

    frame_timer = make_unique<AsyncTimer<uint8_t>>(config.led_controller_frame_delay);
    brightness = make_unique<Brightness>(config.brightness_transition_delay, 0, 0);
    led_mode_controller = make_unique<LedModeController>(this);

    frame_timer->initiate();
}

void LedStripNew::begin_routines_init(const ModuleConfig& cfg) {
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

void LedStripNew::begin_routines_regular(const ModuleConfig& cfg) {
    controller.nvs.sync_from_memory({true, false, false, false, false});
}

void LedStripNew::begin_routines_common(const ModuleConfig& cfg) {
    controller.serial_port.print("Setting up LED lights");
    run_with_dots([this] { loop(); }, (float) color_transition_delay * 1.2f);
}

void LedStripNew::loop() {
    if (frame_timer->is_active()) return;
    frame_timer->reset();
    frame_timer->initiate();

    led_mode_controller->loop();
    fill_all();
}

void LedStripNew::reset(const bool verbose, const bool do_restart, const bool keep_enabled) {
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

string LedStripNew::status(const bool verbose) const {
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

void LedStripNew::set_rgb(const array<uint8_t, 3> new_rgb) {
    led_mode_controller->set_rgb(new_rgb);
}

void LedStripNew::set_r(const uint8_t r) {
    array<uint8_t, 3> old_rgb = get_rgb();
    led_mode_controller->set_rgb({r, old_rgb[1], old_rgb[2]});
}

void LedStripNew::set_g(const uint8_t g) {
    array<uint8_t, 3> old_rgb = get_rgb();
    led_mode_controller->set_rgb({g, old_rgb[1], old_rgb[2]});
}

void LedStripNew::set_b(const uint8_t b) {
    // TODO: Set Blue
}

void LedStripNew::set_hsv(const array<uint8_t, 3> new_hsv) {
    CHSV hsv_buffer(new_hsv);
    hsv2rgb_spectrum(hsv_buffer, rgb_buffer);

    array<uint8_t, 3> new_rgb =
    led_mode_controller->set_rgb(new_rgb);
}

void LedStripNew::set_h(const uint8_t h) {
    array<uint8_t, 3> old_hsv = get_hsv();
    array<uint8_t, 3> new_rgb = LedModeController::hsv_to_rgb({});
    led_mode_controller->set_rgb(new_rgb);
    // TODO: Set Hue
}

void LedStripNew::set_s(const uint8_t s) {
    // TODO: Set Saturation
}

void LedStripNew::set_v(const uint8_t v) {
    // TODO: Set Value
}

array<uint8_t, 3> LedStripNew::get_rgb() const {
    // TODO: Return current RGB
    return {0, 0, 0};
}

uint8_t LedStripNew::get_r() const {
    // TODO: Return Red
    return 0;
}

uint8_t LedStripNew::get_g() const {
    // TODO: Return Green
    return 0;
}

uint8_t LedStripNew::get_b() const {
    // TODO: Return Blue
    return 0;
}

array<uint8_t, 3> LedStripNew::get_hsv() const {
    // TODO: Return current HSV
    return {0, 0, 0};
}

uint8_t LedStripNew::get_h() const {
    // TODO: Return Hue
    return 0;
}

uint8_t LedStripNew::get_s() const {
    // TODO: Return Saturation
    return 0;
}

uint8_t LedStripNew::get_v() const {
    // TODO: Return Value
    return 0;
}

array<uint8_t, 3> LedStripNew::get_target_rgb() const {
    // TODO: Return target RGB
    return {0, 0, 0};
}

uint8_t LedStripNew::get_target_r() const {
    // TODO: Return target Red
    return 0;
}

uint8_t LedStripNew::get_target_g() const {
    // TODO: Return target Green
    return 0;
}

uint8_t LedStripNew::get_target_b() const {
    // TODO: Return target Blue
    return 0;
}

array<uint8_t, 3> LedStripNew::get_target_hsv() const {
    // TODO: Return target HSV
    return {0, 0, 0};
}

uint8_t LedStripNew::get_target_h() const {
    // TODO: Return target Hue
    return 0;
}

uint8_t LedStripNew::get_target_s() const {
    // TODO: Return target Saturation
    return 0;
}

uint8_t LedStripNew::get_target_v() const {
    // TODO: Return target Value
    return 0;
}

// =============================================================================
// Custom Methods: Brightness
// =============================================================================

void LedStripNew::set_brightness(const uint8_t new_brightness) {
    // TODO: Set brightness
}

uint8_t LedStripNew::get_brightness() const {
    // TODO: Return current brightness
    return 0;
}

uint8_t LedStripNew::get_target_brightness() const {
    // TODO: Return target brightness
    return 0;
}

// =============================================================================
// Custom Methods: State
// =============================================================================

void LedStripNew::set_state(const uint8_t state) {
    // TODO: Set state
}

void LedStripNew::toggle_state() {
    // TODO: Toggle state
}

void LedStripNew::turn_on() {
    // TODO: Turn on
}

void LedStripNew::turn_off() {
    // TODO: Turn off
}

bool LedStripNew::get_state() const {
    // TODO: Return state
    return false;
}

bool LedStripNew::get_target_state() const {
    // TODO: Return target state
    return false;
}

// =============================================================================
// Custom Methods: Mode
// =============================================================================

void LedStripNew::set_mode(const uint8_t new_mode) {
    // TODO: Set mode
}

uint8_t LedStripNew::get_mode() const {
    // TODO: Return mode
    return 0;
}

uint8_t LedStripNew::get_target_mode() const {
    // TODO: Return target mode
    return 0;
}

string LedStripNew::get_target_mode_name() const {
    // TODO: Return target mode name
    return "";
}

string LedStripNew::get_all_modes() const {
    // TODO: Return all modes
    return "";
}

// =============================================================================
// Custom Methods: Length
// =============================================================================

void LedStripNew::set_length(const uint16_t length) {
    // TODO: Set length
}

uint16_t LedStripNew::get_length() const {
    // TODO: Return length
    return 0;
}

