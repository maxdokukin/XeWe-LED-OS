#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <Arduino.h>

class SerialPort {
public:
    // ctor: initialize Serial at given baud (default 115200)
    SerialPort(unsigned long baud = 115200);

    // print with newline
    void println(const String &msg);

    // print without newline
    void print(const String &msg);

    // read a full line from Serial and parse it as int
    int get_int();

    // read a full line (until '\n') from Serial
    String get_string();

    // read a full line and interpret ["y","yes","1","true"] → true
    bool get_confirmation();

private:
    unsigned long _baud;

    // discard any pending characters in the buffer
    void flushInput();
};

#endif // SERIALPORT_H
