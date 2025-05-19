// memory.h
#ifndef MEMORY_H
#define MEMORY_H

#include <Arduino.h>
#include <EEPROM.h>

class Memory {
public:
    explicit Memory(size_t size = 512);

    void write_str(const String& key, const String& value);
    String read_str(const String& key) const;

    void write_bit(const String& key, uint8_t bit, bool value);
    bool read_bit(const String& key, uint8_t bit) const;
};

#endif // MEMORY_H
