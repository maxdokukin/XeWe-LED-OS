// Mode.h
#pragma once

#include <array>
#include <cstdint>

using std::array;

class Mode {
public:
    virtual                     ~Mode                        () = default;

    virtual void                loop                        () = 0;

    virtual void                set_rgb                     (const array<uint8_t, 3> new_rgb) = 0;
    virtual array<uint8_t, 3>   get_rgb                     () const = 0;

    virtual uint8_t             get_id                      () const = 0;
    virtual const char*         get_name                    () const = 0;
};
