// memory.cc
#include "Memory.h"

// maximum string length (in bytes) for any key
static constexpr uint8_t MAX_VALUE_SIZE = 32;

// EEPROM address map (bytes)
static constexpr int ADDR_WIFI_FLAGS = 0;                      // 1 byte for 8 flags
static constexpr int ADDR_WIFI_NAME  = ADDR_WIFI_FLAGS + 1;    // next byte is length, then up to MAX_VALUE_SIZE chars
static constexpr int ADDR_WIFI_PASS  = ADDR_WIFI_NAME  + 1 + MAX_VALUE_SIZE;

static int getAddress(const String &key) {
    if      (key == "wifi_flags") return ADDR_WIFI_FLAGS;
    else if (key == "wifi_name")  return ADDR_WIFI_NAME;
    else if (key == "wifi_pass")  return ADDR_WIFI_PASS;
    // unknown key!
    return -1;
}

Memory::Memory(size_t size) {
    EEPROM.begin(size);
}

void Memory::write_str(const String &key, const String &value) {
    int addr = getAddress(key);
    if (addr < 0) return;            // invalid key
    uint8_t len = min((int)value.length(), (int)MAX_VALUE_SIZE);
    EEPROM.write(addr, len);
    for (uint8_t i = 0; i < len; ++i) {
        EEPROM.write(addr + 1 + i, value[i]);
    }
    EEPROM.commit();
}

String Memory::read_str(const String &key) {
    int addr = getAddress(key);
    if (addr < 0) return "";
    uint8_t len = EEPROM.read(addr);
    String result;
    for (uint8_t i = 0; i < len; ++i) {
        result += char(EEPROM.read(addr + 1 + i));
    }
    return result;
}

void Memory::write_bit(const String &key, uint8_t bit, bool value) {
    int addr = getAddress(key);
    if (addr < 0) return;            // invalid key
    uint8_t byte = EEPROM.read(addr);
    if (value)      byte |=  (1 << bit);
    else            byte &= ~(1 << bit);
    EEPROM.write(addr, byte);
    EEPROM.commit();
}

bool Memory::read_bit(const String &key, uint8_t bit) {
    int addr = getAddress(key);
    if (addr < 0) return false;
    uint8_t byte = EEPROM.read(addr);
    return (byte >> bit) & 0x01;
}
