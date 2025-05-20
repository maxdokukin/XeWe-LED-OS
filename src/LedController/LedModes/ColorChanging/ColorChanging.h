#ifndef COLORCHANGING_H
#define COLORCHANGING_H

#include "../LedMode.h"
#include "../../LedController.h"

class ColorChanging : public LedMode {
private:
    AsyncTimerArray* timer;

public:
    ColorChanging(LedController* controller, uint8_t current_r, uint8_t current_g, uint8_t current_b, uint8_t target_r, uint8_t target_g, uint8_t target_b, uint32_t time);

    void frame() override;
    bool is_done() override;
    uint8_t get_mode_id() override;
};

#endif  // ColorChanging_H
