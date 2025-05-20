#ifndef WIFI_H
#define WIFI_H

#include <Arduino.h>
#include <WiFi.h>
#include <vector>

class Wifi {
public:
    // Constructor: set hostname, clear old credentials
    explicit Wifi(const char* hostname = "ESP32-C3-Device");

    // Scan for SSIDs and return their names
    std::vector<String> get_available_networks();

    // Attempt to connect; returns true on success
    bool connect(const String& ssid, const String& password);

    // Disconnect from WiFi; returns true when fully disconnected
    bool disconnect();

    // Returns whether currently connected
    bool is_connected() const;

    // Returns the local IP address as a String ("a.b.c.d")
    String get_local_ip() const;

    // Returns the SSID of the currently connected network (empty if none)
    String get_ssid() const;

    // Returns the MAC address of the WiFi interface ("AA:BB:CC:DD:EE:FF")
    String get_mac_address() const;
};

#endif // WIFI_H
