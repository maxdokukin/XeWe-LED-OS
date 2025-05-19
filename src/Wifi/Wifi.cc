#include "Wifi.h"

Wifi::Wifi(const char* hostname) {
    // Operate as station and set hostname
    WiFi.mode(WIFI_STA);
    WiFi.setHostname(hostname);

    // Clear any previously stored credentials
    WiFi.disconnect(true);
    delay(100);
}

bool Wifi::connect(const String& ssid, const String& password) {
    WiFi.begin(ssid.c_str(), password.c_str());

    unsigned long start_time = millis();
    constexpr unsigned long timeout_ms = 10'000;  // 10 seconds

    // Wait for connection or timeout
    while (millis() - start_time < timeout_ms) {
        if (WiFi.status() == WL_CONNECTED) {
            return true;
        }
        delay(200);
    }
    return false;
}

bool Wifi::disconnect() {
    WiFi.disconnect();

    unsigned long start_time = millis();
    constexpr unsigned long timeout_ms = 5'000;  // 5 seconds

    // Wait until fully disconnected or timeout
    while (WiFi.status() != WL_DISCONNECTED && millis() - start_time < timeout_ms) {
        delay(100);
    }
    return (WiFi.status() == WL_DISCONNECTED);
}

std::vector<String> Wifi::get_available_networks() {
    // Start an async scan (including hidden SSIDs)
    int num_networks = WiFi.scanNetworks(/* async */ true, /* show_hidden */ true);

    // Wait for scan to complete
    while (WiFi.scanComplete() == WIFI_SCAN_RUNNING) {
        delay(10);
    }
    num_networks = WiFi.scanComplete();

    // Collect SSID names
    std::vector<String> ssid_list;
    ssid_list.reserve(num_networks);
    for (int i = 0; i < num_networks; ++i) {
        ssid_list.push_back(WiFi.SSID(i));
    }
    WiFi.scanDelete();

    return ssid_list;
}

bool Wifi::is_connected() const {
    return (WiFi.status() == WL_CONNECTED);
}

String Wifi::get_local_ip() const {
    return WiFi.localIP().toString();
}
