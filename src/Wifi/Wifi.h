#ifndef WIFI_H
#define WIFI_H

#include <Arduino.h>
#include <WiFi.h>
#include <vector>

class Wifi {
public:
    Wifi(const char* hostname = "ESP32-C3-Device");

    // Scan for SSIDs and return their names
    std::vector<String> get_available_networks();

    // Attempt to connect; returns true on success
    bool connect(const String &ssid, const String &password);

    // Returns whether currently connected
    bool is_connected() const { return WiFi.status() == WL_CONNECTED; }

    // Returns the local IP address as a String (e.g. "192.168.0.42")
    String get_local_ip() const;
};

#endif // WIFI_H
