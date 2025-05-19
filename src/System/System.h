// System.h
#ifndef SYSTEM_H
#define SYSTEM_H

#include <Arduino.h>
#include <vector>
#include "../Interfaces/SerialPort/SerialPort.h"
#include "../Wifi/Wifi.h"
#include "../Memory/Memory.h"
#include "CommandParser/CommandParser.h"

class System {
public:
    System();
    void init_system_setup();
    void update();

private:
    void define_commands();

    // Wi-Fi flows
    bool connect_wifi();
    bool read_memory_wifi_credentials(String& ssid, String& pwd);
    bool prompt_user_for_wifi_credentials(String& ssid, String& pwd);
    bool disconnect_wifi();
    bool reset_wifi_credentials();

    SerialPort                     serial_port;
    Wifi                           wifi;
    Memory                         memory;
    bool                           wifi_connected = false;
    CommandParser                  command_parser;

    static const size_t            WIFI_CMD_COUNT = 5;
    CommandParser::Command         wifi_commands[WIFI_CMD_COUNT];
    CommandParser::CommandGroup    wifi_group;
};

#endif // SYSTEM_H
