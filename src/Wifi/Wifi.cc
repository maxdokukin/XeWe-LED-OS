#include "Wifi.h"

Wifi::Wifi(const char* hostname) {
    WiFi.mode(WIFI_STA);
    WiFi.setHostname(hostname);
    WiFi.disconnect(true);   // clear old credentials
    delay(100);
}

std::vector<String> Wifi::get_available_networks() {
    std::vector<String> nets;
    int n = WiFi.scanNetworks(true, true);  // async scan, show hidden
    while (WiFi.scanComplete() == WIFI_SCAN_RUNNING) {
        delay(10);
    }
    n = WiFi.scanComplete();
    for (int i = 0; i < n; ++i) {
        nets.push_back(WiFi.SSID(i));
    }
    WiFi.scanDelete();
    return nets;
}

bool Wifi::connect(const String &ssid, const String &password) {
    WiFi.begin(ssid.c_str(), password.c_str());
    unsigned long start = millis();
    // wait up to 10s
    while (millis() - start < 10000) {
        if (WiFi.status() == WL_CONNECTED) {
            return true;
        }
        delay(200);
    }
    return false;
}
