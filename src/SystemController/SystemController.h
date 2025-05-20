// SystemController.h
#ifndef SYSTEMCONTROLLER_H
#define SYSTEMCONTROLLER_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <vector>
#include "../Interfaces/SerialPort/SerialPort.h"
#include "../Interfaces/Wifi/Wifi.h"
#include "../Resources/Memory/Memory.h"
#include "../LedController/LedController.h"
#include "CommandParser/CommandParser.h"

class SystemController {
public:
    SystemController(Adafruit_NeoPixel* strip);
    void init_system_setup();
    void update();

private:
    void define_commands();

    // Wi-Fi flows
    void print_wifi_credentials();
    void print_wifi_help();
    std::vector<String> get_available_wifi_networks();

    bool connect_wifi();
    bool read_memory_wifi_credentials(String& ssid, String& pwd);
    bool prompt_user_for_wifi_credentials(String& ssid, String& pwd);
    bool disconnect_wifi();
    bool reset_wifi_credentials();

    SerialPort                     serial_port;
    Wifi                           wifi;
    Memory                         memory;
    CommandParser                  command_parser;
    LedController                  led_controller;

    static const size_t            WIFI_CMD_COUNT = 6;
    CommandParser::Command         wifi_commands[WIFI_CMD_COUNT];
    CommandParser::CommandGroup    wifi_group;
};

#endif // SYSTEMCONTROLLER_H
