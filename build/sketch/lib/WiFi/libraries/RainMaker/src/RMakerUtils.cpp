#line 1 "/Users/xewe/Documents/Programming/Arduino/XeWe-LedOS/lib/WiFi/libraries/RainMaker/src/RMakerUtils.cpp"
#include "RMakerUtils.h"
#ifdef CONFIG_ESP_RMAKER_WORK_QUEUE_TASK_STACK
#define RESET_DELAY_SEC 2
void RMakerFactoryReset(int reboot_seconds) {
  esp_rmaker_factory_reset(RESET_DELAY_SEC, reboot_seconds);
}

void RMakerWiFiReset(int reboot_seconds) {
  esp_rmaker_wifi_reset(RESET_DELAY_SEC, reboot_seconds);
}
#endif
