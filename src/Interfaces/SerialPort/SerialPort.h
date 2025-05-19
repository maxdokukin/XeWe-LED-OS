#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <Arduino.h>

class SerialPort {
public:
    SerialPort(unsigned long baud = 115200);

    void println(const String& msg);
    void print(const String& msg);

    // True if there's a full '\n'-terminated line waiting
    bool has_line();

    // Blocking: read and return the next line (no '\n', trimmed)
    String read_line();

    // Blocking: flush old input and read one line
    String get_string();

    // Blocking: read one line and parse as integer
    int get_int();

    // Blocking: read one line and interpret yes/no
    bool get_confirmation();

private:
    unsigned long _baud;

    // Discard any buffered bytes (e.g. leftover '\n')
    void flush_input();
};

#endif // SERIALPORT_H
