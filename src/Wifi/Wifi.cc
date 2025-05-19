#include "Wifi.h"

Wifi::Wifi(const char* hostname) {
    // Operate as station and set hostname
    WiFi.mode(WIFI_STA);
    WiFi.setHostname(hostname);

    // Clear any previously stored credentials
    WiFi.disconnect(true);
    delay(100);
}

bool Wifi::disconnect() {
    // Issue the disconnect (and optionally erase saved creds)
    WiFi.disconnect();

    // Wait up to 5 seconds for disconnection
    unsigned long start = millis();
    while (WiFi.status() != WL_DISCONNECTED && millis() - start < 5000) {
        delay(100);
    }

    // Return whether we succeeded in disconnecting
    return (WiFi.status() == WL_DISCONNECTED);
}

std::vector<String> Wifi::get_available_networks() {
    std::vector<String> nets;

    // Start an async scan (including hidden SSIDs)
    int n = WiFi.scanNetworks(true, true);
    while (WiFi.scanComplete() == WIFI_SCAN_RUNNING) {
        delay(10);
    }
    n = WiFi.scanComplete();

    // Collect SSID names
    for (int i = 0; i < n; ++i) {
        nets.push_back(WiFi.SSID(i));
    }
    WiFi.scanDelete();
    return nets;
}

bool Wifi::connect(const String &ssid, const String &password) {
    WiFi.begin(ssid.c_str(), password.c_str());
    unsigned long start = millis();

    // Wait up to 10 seconds for connection
    while (millis() - start < 10000) {
        if (WiFi.status() == WL_CONNECTED) {
            return true;
        }
        delay(200);
    }
    return false;
}

String Wifi::get_local_ip() const {
    // Convert IPAddress to String "a.b.c.d"
    return WiFi.localIP().toString();
}
