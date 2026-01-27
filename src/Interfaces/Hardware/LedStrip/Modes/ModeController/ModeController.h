#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <FastLED.h>

using std::array;
using std::string;
using std::unique_ptr;

class Mode;
template <typename T> class AsyncTimer;

class ModeController {
public:
    explicit                    ModeController              (uint16_t mode_transition_delay);

    CRGB*                       loop                        ();

    void                        set_rgb                     (const array<uint8_t, 3> new_rgb);
    array<uint8_t, 3>           get_rgb                     () const;

    void                        set_mode                    (const uint8_t new_mode);
    uint8_t                     get_mode                    () const;
    string                      get_mode_name               () const;
    string                      get_all_modes               () const;

    static std::array<uint8_t, 3> hsv_to_rgb                (uint8_t h, uint8_t s, uint8_t v);
    static std::array<uint8_t, 3> rgb_to_hsv                (uint8_t r, uint8_t g, uint8_t b);

private:
    using MakeFn = unique_ptr<Mode>(*)(const array<uint8_t, 3>&);

    struct ModeDesc {
        uint8_t                 id;
        const char*             name;
        MakeFn                  make;
    };

    std::array<ModeDesc, 3>     mode_registry;

    unique_ptr<Mode>            current_mode;
    unique_ptr<Mode>            old_mode;

    unique_ptr<AsyncTimer<uint16_t>> transition_timer;

    CRGB                        frame[LED_STRIP_NUM_LEDS_MAX];

    const ModeDesc*             find_mode                   (uint8_t id) const;
    void                        begin_transition            (unique_ptr<Mode> next);
};
