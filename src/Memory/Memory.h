#ifndef MEMORY_H
#define MEMORY_H

#include <Arduino.h>
#include <EEPROM.h>

class Memory {
public:
    Memory(size_t size = 512);
    void write(const String &key, const String &value);
    String read(const String &key);
};
#endif // MEMORY_H
