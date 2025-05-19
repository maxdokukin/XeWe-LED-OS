#include "SerialPort.h"

SerialPort::SerialPort(unsigned long baud)
  : _baud(baud)
{
    Serial.begin(_baud);
    // give USB-CDC a moment (this is milliseconds, not seconds)
    delay(2);
}

void SerialPort::flush_input() {
    while (Serial.available()) {
        Serial.read();
        yield();
    }
}

void SerialPort::println(const String& msg) {
    Serial.println(msg);
}

void SerialPort::print(const String& msg) {
    Serial.print(msg);
}

bool SerialPort::has_line() {
      return Serial.available() > 0;

}

String SerialPort::read_line() {
  // block until we see a newline:
  String line;
  while (true) {
    if (Serial.available()) {
      char c = Serial.read();
      yield();
      if (c == '\n') break;
      if (c != '\r') line += c;
    } else {
      yield();
    }
  }
  line.trim();      // void
  return line;      // now we always return
}


String SerialPort::get_string() {
    // flush any stray bytes before reading
    flush_input();

    // then block for a fresh full line
    return read_line();
}

int SerialPort::get_int() {
    String s = get_string();
    while (s.length() == 0) {
        s = get_string();
    }
    return s.toInt();
}

bool SerialPort::get_confirmation() {
    String s = get_string();
    s.trim();
    s.toLowerCase();
    return (s == "y" || s == "yes" || s == "1" || s == "true");
}
