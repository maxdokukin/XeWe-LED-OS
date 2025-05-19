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

//    wifi
    bool connect_wifi();
    bool read_memory_wifi_credentials(String& ssid, String& pwd);
    bool prompt_user_for_wifi_credentials(String& ssid, String& pwd);
    bool disconnect_wifi();
    bool reset_wifi_credentials();

    SerialPort                        serialPort;
    Wifi                              wifi;
    Memory                            memory;
    bool                              wifi_connected = false;
    CommandParser                     command_parser;

    // Increase to 5 so we can index 0…4
    static const size_t WIFI_CMD_COUNT = 5;
    CommandParser::Command      wifiCommands[WIFI_CMD_COUNT];
    CommandParser::CommandGroup wifiGroup;
};

#endif // SYSTEM_H
