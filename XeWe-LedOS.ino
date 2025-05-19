#include "src/System/System.h"
#include <Arduino.h>

System * led_os = nullptr;

void setup() {
  // Serial.begin(115200);
  // delay(2);
  // Serial.println("Hello");

  led_os = new System();
  led_os->init_system_setup();

}

void loop() {
  // put your main code here, to run repeatedly:
}
