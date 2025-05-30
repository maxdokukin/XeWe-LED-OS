#line 1 "/Users/xewe/Documents/Programming/Arduino/XeWe-LedOS/lib/WiFi/variants/lolin_s3_mini_pro/variant.cpp"
/*
 */

#include "esp32-hal-gpio.h"
#include "pins_arduino.h"

extern "C" {

// Initialize variant/board, called before setup()
void initVariant(void) {
  // IR
  pinMode(PIN_IR, OUTPUT);
  digitalWrite(PIN_IR, LOW);
  // RGB
  pinMode(RGB_POWER, OUTPUT);
  digitalWrite(RGB_POWER, LOW);
  // BUTTON
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_OK, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
  // TFT
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, LOW);
  pinMode(TFT_CS, OUTPUT);
  digitalWrite(TFT_CS, HIGH);
  pinMode(TFT_RST, OUTPUT);
  digitalWrite(TFT_RST, LOW);
  delay(1);
  digitalWrite(TFT_RST, HIGH);
}
}
