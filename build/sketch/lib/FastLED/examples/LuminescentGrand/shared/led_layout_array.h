#line 1 "/Users/xewe/Documents/Programming/Arduino/XeWe-LedOS/lib/FastLED/examples/LuminescentGrand/shared/led_layout_array.h"

#ifndef LED_ARRAY_H_
#define LED_ARRAY_H_

struct LedColumns {
  LedColumns(const int* a, int l) : array(a), length(l) {}
  LedColumns(const LedColumns& other) : array(other.array), length(other.length) {}
  const int* array;
  int length;
};

LedColumns LedLayoutArray();

#endif  // LED_ARRAY_H_
