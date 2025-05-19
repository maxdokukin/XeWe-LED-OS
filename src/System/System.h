// System.h
#ifndef SYSTEM_H
#define SYSTEM_H

#include "../Interfaces/SerialPort/SerialPort.h"
#include "../Wifi/Wifi.h"
#include "../Memory/Memory.h"
#include "CommandParser/CommandParser.h"

class System {
public:
    System();

    void init_system_setup();

private:
    // Loop until we’ve successfully connected
    bool connect_wifi();

    // Try to read SSID/PWD from memory; returns true if connect succeeds.
    bool read_memory_wifi_credentials(String& ssid, String& pwd);

    // Prompt user, scan networks, fill ssid/pwd; returns true if user provided creds.
    bool prompt_user_for_wifi_credentials(String& ssid, String& pwd);

    SerialPort serialPort;
    Wifi                wifi;
    Memory              memory;
    CommandParser       command_parser;
    bool                wifi_connected;
};

#endif // SYSTEM_H
