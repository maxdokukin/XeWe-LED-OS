#include "Memory.h"

// EEPROM address map
static constexpr int ADDR_WIFI_NAME = 0;
static constexpr int ADDR_WIFI_PASS = 128;
static constexpr uint8_t MAX_VALUE_SIZE = 127;

static int getAddress(const String &key) {
    if (key == "wifi_name") return ADDR_WIFI_NAME;
    if (key == "wifi_pass") return ADDR_WIFI_PASS;
    return 0;
}

Memory::Memory(size_t size) {
    EEPROM.begin(size);   // allocate flash-backed EEPROM
}

void Memory::write(const String &key, const String &value) {
    int addr = getAddress(key);
    uint8_t len = min((int)value.length(), (int)MAX_VALUE_SIZE);
    EEPROM.write(addr, len);
    for (uint8_t i = 0; i < len; ++i) {
        EEPROM.write(addr + 1 + i, value[i]);
    }
    EEPROM.commit();
}

String Memory::read(const String &key) {
    int addr = getAddress(key);
    uint8_t len = EEPROM.read(addr);
    String result;
    for (uint8_t i = 0; i < len; ++i) {
        result += char(EEPROM.read(addr + 1 + i));
    }
    return result;
}
