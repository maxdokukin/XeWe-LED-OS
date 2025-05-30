#line 1 "/Users/xewe/Documents/Programming/Arduino/XeWe-LedOS/lib/WiFi/cores/esp32/base64.h"
#ifndef CORE_BASE64_H_
#define CORE_BASE64_H_

class base64 {
public:
  static String encode(const uint8_t *data, size_t length);
  static String encode(const String &text);

private:
};

#endif /* CORE_BASE64_H_ */
