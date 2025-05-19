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
    // Wi-Fi flows
    bool connect_wifi();
    bool read_memory_wifi_credentials(String& ssid, String& pwd);
    bool prompt_user_for_wifi_credentials(String& ssid, String& pwd);

    SerialPort                        serialPort;
    Wifi                        wifi;
    Memory                            memory;
    bool                              wifi_connected = false;

    CommandParser                     command_parser;

    // Fixed-size array for wifi commands
    static const size_t WIFI_CMD_COUNT = 3;
    CommandParser::Command      wifiCommands[WIFI_CMD_COUNT];
    CommandParser::CommandGroup wifiGroup;
};

#endif // SYSTEM_H

