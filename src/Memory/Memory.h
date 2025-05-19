// memory.h
#ifndef MEMORY_H
#define MEMORY_H

#include <Arduino.h>
#include <EEPROM.h>

class Memory {
public:
    // ctor: total EEPROM size in bytes (default 512)
    Memory(size_t size = 512);

    // write and read arbitrary strings (max 127 chars)
    void write_str(const String &key, const String &value);
    String read_str(const String &key);

    // write/read a single bit within the byte at the key's address
    void write_bit(const String &key, uint8_t bit, bool value);
    bool read_bit(const String &key, uint8_t bit);
};

#endif // MEMORY_H
