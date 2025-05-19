#include "SerialPort.h"

SerialPort::SerialPort(unsigned long baud)
    : _baud(baud)
{
    Serial.begin(_baud);
    delay(2);
}

void SerialPort::flushInput() {
    while (Serial.available()) {
        Serial.read();
    }
}

void SerialPort::println(const String &msg) {
    Serial.println(msg);
}

void SerialPort::print(const String &msg) {
    Serial.print(msg);
}

String SerialPort::get_string() {
    // clear any leftover data (e.g. stray '\n')
    flushInput();

    // wait for user to send something ending in '\n'
    while (!Serial.available()) {}
    String line = Serial.readStringUntil('\n');
    // strip trailing \r or spaces
    line.trim();
    return line;
}

int SerialPort::get_int() {
    // read a clean line, then convert
    String line = get_string();
    // if the user just hit Enter, keep waiting
    while (line.length() == 0) {
        line = get_string();
    }
    return line.toInt();
}

bool SerialPort::get_confirmation() {
    String resp = get_string();
    // normalize case
    resp.toLowerCase();
    return (resp == "y" || resp == "yes" || resp == "1" || resp == "true");
}
