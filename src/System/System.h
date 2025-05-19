#ifndef SYSTEM_H
#define SYSTEM_H

#include "../Interfaces/SerialPort/SerialPort.h"
#include "../Wifi/Wifi.h"
#include "../Memory/Memory.h"

class System {
public:
    System();

    void init_system_setup();

private:
    // Loop until we’ve successfully connected
    bool connect_wifi();

    SerialPort serialPort;
    Wifi       wifi;
    Memory     memory;
    bool       wifi_connected;
};

#endif // SYSTEM_H
