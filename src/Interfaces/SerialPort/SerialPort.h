#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <Arduino.h>

class SerialPort {
public:
    // ctor: baud rate (default 115200)
    SerialPort(unsigned long baud = 115200);

    // print with newline
    void println(const String &msg);

    // print without newline
    void print(const String &msg);

    // read and return the next integer from serial
    int get_int();

    // read until newline and return the resulting string
    String get_string();

    // read a confirmation (“y”/“yes” → true; else false)
    bool get_confirmation();

private:
    unsigned long _baud;
};

#endif // SERIALPORT_H
