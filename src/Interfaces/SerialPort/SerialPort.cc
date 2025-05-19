#include "SerialPort.h"

SerialPort::SerialPort(unsigned long baud)
  : _baud(baud)
{
    Serial.begin(_baud);
    delay(2);
}

void SerialPort::println(const String &msg) {
    Serial.println(msg);
}

void SerialPort::print(const String &msg) {
    Serial.print(msg);
}

int SerialPort::get_int() {
    // wait until data arrives
    while (!Serial.available()) {
        delay(10);
    }
    return Serial.parseInt();
}

String SerialPort::get_string() {
    // wait until data arrives
    while (!Serial.available()) {
        delay(10);
    }
    return Serial.readStringUntil('\n');
}

bool SerialPort::get_confirmation() {
    String resp = get_string();
    resp.trim();
    resp.toLowerCase();
    return (resp == "y" || resp == "yes" || resp == "1" || resp == "true");
}
