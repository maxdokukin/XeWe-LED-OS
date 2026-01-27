#pragma once

#include <array>
#include <string>
#include <vector>
#include <cstdint>
#include <FastLED.h>

class Mode {
public:
    /**
     * @brief Construct a new Mode object
     * @param num_leds The number of LEDs on the strip (for buffer sizing)
     * @param id The unique ID of the mode (must match registry)
     * @param name The display name
     * @param rgb The initial color state
     */
    Mode(uint16_t num_leds, uint8_t id, const std::string& name, const std::array<uint8_t, 3>& rgb)
        : _num_leds(num_leds), _id(id), _name(name), _rgb(rgb) {

        // Dynamically size the buffer
        _leds.resize(_num_leds);
        // Initialize to black
        fill_solid(_leds.data(), _num_leds, CRGB::Black);
    }

    virtual ~Mode() = default;

    /**
     * @brief Run animation logic.
     * @return pointer to the start of the CRGB array (data).
     */
    virtual const CRGB* loop() = 0;

    // Getters
    uint8_t get_id() const { return _id; }
    std::string get_name() const { return _name; }
    std::array<uint8_t, 3> get_rgb() const { return _rgb; }

protected:
    // Helper to update stored RGB state if the animation changes it
    void set_rgb(const std::array<uint8_t, 3>& new_rgb) {
        _rgb = new_rgb;
    }

    // Dynamic buffer
    std::vector<CRGB> _leds;

    // Strip length
    uint16_t _num_leds;

    // State
    uint8_t _id;
    std::string _name;
    std::array<uint8_t, 3> _rgb;
};