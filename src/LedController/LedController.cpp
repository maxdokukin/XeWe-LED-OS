#include "LedController.h"

LedController::LedController(Adafruit_NeoPixel* strip)
    : led_strip(strip) {

    led_strip->begin();
    led_strip->setBrightness(255);

    //    Functional classes
    frame_timer = new AsyncTimer<uint8_t>(led_controller_frame_delay);
    // read from memory mode
    led_mode = new ColorSolid(this, init_r, init_g, init_b);
    led_mode->set_rgb(init_r, init_g, init_b);
    led_mode->rgb_to_hsv();

    brightness = new Brightness(this, init_brightness, brightness_transition_delay);

    Serial.println("LedController: Constructor called");
}

void LedController::frame() {
    // Call the terminal frame method
    if(frame_timer->is_active())
        return;
    frame_timer->reset();

    brightness->frame();
    led_mode->frame();

    // Switch statement for different LED modes
    switch (led_mode->get_mode_id()) {
        case 1:
            break;

        case 2:
            if (led_mode->is_done()){
                led_mode->frame();
                delete led_mode;
                led_mode = new ColorSolid(this);
            }
            break;
        case 3:
            // Add logic for DIRECTIONAL_FILLER mode here
            Serial.println("Mode: DIRECTIONAL_FILLER");
            break;

        case 4:
            // Add logic for PERLIN mode here
            Serial.println("Mode: PERLIN");
            break;

        case 0:
            // Add logic for OFF mode here
            Serial.println("Mode: OFF");
            break;

        default:
            Serial.println("Unknown mode");
            break;
    }
}


void LedController::set_mode(uint8_t new_mode) {
    Serial.printf("LedController: Function: set_mode, mode = %d\n", new_mode);
}

void LedController::set_rgb(uint8_t r, uint8_t g, uint8_t b) {
    Serial.printf("LedController: Function: set_rgb, R = %d, G = %d, B = %d\n", r, g, b);
    if (!led_mode || !led_mode->is_done()) {
        Serial.println("LedController: Still changing previous color or led_mode is null");
        return;
    }
    if (r == led_mode->get_r() && g == led_mode->get_g() && b == led_mode->get_b()) {
        Serial.println("LedController: Color already set");
        return;
    }
    uint8_t old_r = led_mode->get_r();
    uint8_t old_g = led_mode->get_g();
    uint8_t old_b = led_mode->get_b();
    delete led_mode;
    led_mode = new ColorChanging(this, old_r, old_g, old_b, r, g, b, color_transition_delay);
}

void LedController::set_r(uint8_t r) {
    Serial.printf("LedController: Function: set_r, R = %d\n", r);
    set_rgb(r, led_mode->get_g(), led_mode->get_b());
}

void LedController::set_g(uint8_t g) {
    Serial.printf("LedController: Function: set_g, G = %d\n", g);
    set_rgb(led_mode->get_r(), g, led_mode->get_b());
}

void LedController::set_b(uint8_t b) {
    Serial.printf("LedController: Function: set_b, B = %d\n", b);
    set_rgb(led_mode->get_r(), led_mode->get_g(), b);
}

void LedController::set_hsv(uint8_t hue, uint8_t saturation, uint8_t value) {
    Serial.printf("LedController: Function: NOT IMPLEMENTEDset_hsv, H = %d, S = %d, V = %d\n", hue, saturation, value);
    if (!led_mode->is_done()){
        Serial.println("LedController: Still changing previous color");
        return;
    }
    if (hue == led_mode->get_hue() && saturation ==led_mode->get_sat() && value == led_mode->get_val()){
        Serial.println("LedController: HSV already set");
        return;
    }
    uint8_t old_r = led_mode->get_r();
    uint8_t old_g = led_mode->get_g();
    uint8_t old_b = led_mode->get_b();
//    delete led_mode;
//    led_mode = new ColorChanging(this, old_r, old_g, old_b, color_transition_delay);
//    led_mode->set_hsv(hue, saturation, value);
//    led_mode->hsv_to_rgb();
}

void LedController::set_hue(uint8_t hue) {
    set_hsv(hue, led_mode->get_sat(), led_mode->get_val());
}

void LedController::set_sat(uint8_t saturation) {
    set_hsv(led_mode->get_hue(), saturation, led_mode->get_val());
}

void LedController::set_val(uint8_t value) {
    set_hsv(led_mode->get_hue(), led_mode->get_sat(), value);
}

void LedController::set_brightness(uint8_t new_brightness) {
    Serial.printf("LedController: Function: set_brightness, brightness = %d\n", new_brightness);
    if(brightness->get_target_value() == new_brightness){
        Serial.println("LedController: Function: set_brightness: Already set to this value.");
        return;
    }
    if (state) {
        brightness->set_brightness(new_brightness);
    }
}

void LedController::set_state(byte target_state){
    if(target_state)
        turn_on();
    else
        turn_off();
}

void LedController::turn_on() {
    Serial.println("LedController: Function: turn_on");
    if(state){
        Serial.println("Already on");
        return;
    }
    brightness->set_brightness(brightness->get_start_value());
    state = 1;
}

void LedController::turn_off() {
    Serial.println("LedController: Function: turn_off");
    if(!state){
        Serial.println("Already off");
        return;
    }
    state = 0;
    brightness->set_brightness(0);
}

void LedController::fill_all(uint8_t r, uint8_t g, uint8_t b) {
    Serial.printf("LedController: Function: fill_all, R = %d, G = %d, B = %d\n", r, g, b);
    for (int i = 0; i < num_led; i++) {
        set_all_strips_pixel_color(i, r, g, b);
    }
    led_strip->show();
}

void LedController::set_all_strips_pixel_color(uint16_t i, uint8_t r, uint8_t g, uint8_t b) {
    uint8_t current_brightness = brightness->get_current_value();
    uint8_t min_led_value = static_cast<uint8_t>(current_brightness ? 1 : 0); //0 or 1
    uint8_t dimmed_r = led_mode->get_r() ? max(min_led_value, static_cast<uint8_t>((static_cast<uint16_t>(r) * current_brightness) / 255)) : 0;
    uint8_t dimmed_g = led_mode->get_g() ? max(min_led_value, static_cast<uint8_t>((static_cast<uint16_t>(g) * current_brightness) / 255)) : 0;
    uint8_t dimmed_b = led_mode->get_b() ? max(min_led_value, static_cast<uint8_t>((static_cast<uint16_t>(b) * current_brightness) / 255)) : 0;
    if( i == 0)
        Serial.printf("LedController: set_all_strips_pixel_color: fill_all, R = %d, G = %d, B = %d, Br=%d\n", dimmed_r, dimmed_g, dimmed_b, current_brightness);

    led_strip->setPixelColor(i, dimmed_r, dimmed_g, dimmed_b);
//    if (is_zigzag_config) {
//        led_strip->setPixelColor(num_led - i - 1, r, g, b);
//    }
}

