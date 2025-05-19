#include "src/System/System.h"
#include <Arduino.h>

System * led_os = nullptr;

void setup() {
  led_os = new System();
  led_os->init_system_setup();
}

void loop() {
  led_os->update();
}
