// LedModeController.h
#pragma once

#include <array>
#include <FastLED.h>

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

    static std::array<uint8_t, 3> hsv_to_rgb(uint8_t h, uint8_t s, uint8_t v);
    static std::array<uint8_t, 3> rgb_to_hsv(uint8_t r, uint8_t g, uint8_t b);

private:
    unique_ptr                  <Mode>                      current_mode;

    unique_ptr                  <AsyncTimer<uint16_t>>      transition_timer;
};
