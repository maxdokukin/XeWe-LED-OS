#ifndef SERIAL_PORT_H
#define SERIAL_PORT_H

#include <Arduino.h>

class SerialPort {
public:
    explicit SerialPort(unsigned long baud_rate = 115200);

    // Send text without newline
    void print(const String& message);

    // Send text with newline
    void println(const String& message);

    // True if there's at least one byte waiting in the buffer
    bool has_line() const;

    // Blocking: read and return the next '\n'-terminated line (no '\n', trimmed)
    String read_line();

    // Blocking: flush old input then read one line
    String get_string();

    // Blocking: read one line and parse as integer
    int get_int();

    // Blocking: read one line and interpret yes/no
    bool get_confirmation();

    void print_dash_line();
private:
    unsigned long baud_rate_;

    // Discard any buffered bytes (e.g. leftover '\n')
    void flush_input();
};

#endif  // SERIAL_PORT_H
