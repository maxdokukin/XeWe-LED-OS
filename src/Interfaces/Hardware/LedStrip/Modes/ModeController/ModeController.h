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

    void                        loop                        ();

    void                        set_rgb                     (const array<uint8_t, 3> new_rgb);
    array<uint8_t, 3>           get_rgb                     () const;

    void                        set_mode                    (const uint8_t new_mode);
    uint8_t                     get_mode                    () const;
    string                      get_mode_name               () const;
    string                      get_all_modes               () const;

    static std::array<uint8_t, 3> hsv_to_rgb                (uint8_t h, uint8_t s, uint8_t v);
    static std::array<uint8_t, 3> rgb_to_hsv                (uint8_t r, uint8_t g, uint8_t b);

private:
    using MakeFn = unique_ptr<Mode>(*)();

    struct ModeDesc {
        uint8_t                 id;
        const char*             name;
        MakeFn                  make;
    };

    static constexpr size_t     MODE_COUNT = 3;
    <ModeDesc, MODE_COUNT>      mode_registry;

    void                        register_modes              ();
    const ModeDesc*             find_mode                   (uint8_t id) const;

    unique_ptr                  <Mode>                      current_mode;
    unique_ptr                  <AsyncTimer<uint16_t>>      transition_timer;
};
