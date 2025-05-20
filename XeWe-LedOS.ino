#include "src/SystemController/SystemController.h"
#include <Arduino.h>

SystemController * led_os = nullptr;

void setup() {
  led_os = new SystemController();
  led_os->init_system_setup();
}

void loop() {
  led_os->update();
}
