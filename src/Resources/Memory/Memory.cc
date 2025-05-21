// memory.cc
#include "memory.h"

static constexpr uint8_t WIFI_WORD_LEN_MAX = 32;

// EEPROM address map (bytes)
static constexpr int ADDR_WIFI_FLAGS = 0;
static constexpr int ADDR_WIFI_NAME  = ADDR_WIFI_FLAGS + 1;
static constexpr int ADDR_WIFI_PASS  = WIFI_WORD_LEN_MAX  + 1 + WIFI_WORD_LEN_MAX;

static int get_address(const String& key) {
    if (key == "wifi_flags") {
        return ADDR_WIFI_FLAGS;
    } else if (key == "wifi_name") {
        return ADDR_WIFI_NAME;
    } else if (key == "wifi_pass") {
        return ADDR_WIFI_PASS;
    }
    return -1;
}

Memory::Memory(size_t size) {
    EEPROM.begin(size);
}

void Memory::write_str(const String& key, const String& value) {
    int addr = get_address(key);
    if (addr < 0) {
        return;
    }
    uint8_t length = min(static_cast<int>(value.length()), static_cast<int>(WIFI_WORD_LEN_MAX));
    EEPROM.write(addr, length);
    for (uint8_t i = 0; i < length; ++i) {
        EEPROM.write(addr + 1 + i, value[i]);
    }
    EEPROM.commit();
}

String Memory::read_str(const String& key) const {
    int addr = get_address(key);
    if (addr < 0) {
        return "";
    }
    uint8_t length = EEPROM.read(addr);
    String result;
    for (uint8_t i = 0; i < length; ++i) {
        result += char(EEPROM.read(addr + 1 + i));
    }
    return result;
}

void Memory::write_bit(const String& key, uint8_t bit, bool value) {
    int addr = get_address(key);
    if (addr < 0) {
        return;
    }
    uint8_t byte = EEPROM.read(addr);
    if (value) {
        byte |= (1 << bit);
    } else {
        byte &= ~(1 << bit);
    }
    EEPROM.write(addr, byte);
    EEPROM.commit();
}

bool Memory::read_bit(const String& key, uint8_t bit) const {
    int addr = get_address(key);
    if (addr < 0) {
        return false;
    }
    uint8_t byte = EEPROM.read(addr);
    return (byte >> bit) & 0x01;
}
