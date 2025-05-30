#line 1 "/Users/xewe/Documents/Programming/Arduino/XeWe-LedOS/lib/FastLED/src/fl/pair.h"
#pragma once

namespace fl {

template <typename Key, typename Value> struct Pair {
    Key first = Key();
    Value second = Value();
    Pair() = default;
    Pair(const Key &k, const Value &v) : first(k), second(v) {}
};

// std compatibility
template <typename Key, typename Value> using pair = Pair<Key, Value>;

} // namespace fl
