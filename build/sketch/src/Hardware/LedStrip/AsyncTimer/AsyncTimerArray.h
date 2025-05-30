#line 1 "/Users/xewe/Documents/Programming/Arduino/XeWe-LedOS/src/Hardware/LedStrip/AsyncTimer/AsyncTimerArray.h"
#ifndef ASYNCTIMERARRAY_H
#define ASYNCTIMERARRAY_H

#include <array>
#include "../../../Debug.h"

class AsyncTimerArray {
    static_assert(std::is_same_v<uint8_t, std::array<uint8_t,3>::value_type>,
                  "AsyncTimerArray works on 3-byte arrays of uint8_t");

private:
    mutable uint32_t start_time{};
    mutable uint32_t last_calc_time{};
    static constexpr uint32_t calc_interval_ms = 5;

    uint32_t delay_ms;
    std::array<uint8_t,3> start_val{}, target_val{};
    mutable bool done{false}, initiated{false};
    mutable double progress{0.0};

    void calculate_progress() const {
        if (done || !initiated) return;

        uint32_t now = millis();
        if (now - last_calc_time < calc_interval_ms) return;
        last_calc_time = now;

        if (start_val == target_val) {
            done     = true;
            progress = 1.0;
            return;
        }

        uint32_t elapsed = now - start_time;
        progress = double(elapsed) / delay_ms;
        if (progress >= 1.0) {
            done     = true;
            progress = 1.0;
        }
    }

public:
    /**
     * @param delay   Transition duration in milliseconds.
     * @param start   Starting RGB values (default all 0).
     * @param target  Target RGB values (default all 0).
     */
    AsyncTimerArray(uint32_t delay,
                    const std::array<uint8_t,3>& start  = {},
                    const std::array<uint8_t,3>& target = {})
        : delay_ms(delay), start_val(start), target_val(target)
    {}
    ~AsyncTimerArray() = default;

    /** Begin (or restart) the transition. */
    void initiate() {
        start_time     = millis();
        if (start_time > calc_interval_ms)
            last_calc_time = start_time - calc_interval_ms;
        else
            last_calc_time = 0;
        progress       = 0.0;
        done           = false;
        initiated      = true;
    }

    std::array<uint8_t,3> get_start_value() const {
        return start_val;
    }

    /** Current interpolated RGB triple. */
    std::array<uint8_t,3> get_current_value() const {
        calculate_progress();
        std::array<uint8_t,3> cur{};
        for (int i = 0; i < 3; ++i) {
            cur[i] = static_cast<uint8_t>(
                start_val[i] + (target_val[i] - start_val[i]) * progress
            );
        }
        return cur;
    }

    /** Final target RGB triple. */
    std::array<uint8_t,3> get_target_value() const {
        return target_val;
    }

    /** True once interpolation has reached the end. */
    bool is_done() const {
        calculate_progress();
        return done;
    }

    /** True while a transition is in progress. */
    bool is_active() const {
        calculate_progress();
        return initiated && !done;
    }

    /** Stop without completing (keeps current state). */
    void terminate() {
        initiated = false;
    }

    /** Reset to un-initiated state (will recalc on next initiate). */
    void reset() {
        done            = false;
        progress        = 0.0;
        initiated       = false;
        last_calc_time  = 0;
    }

    /** Reset with new start/target but same delay. */
    void reset(const std::array<uint8_t,3>& new_start,
               const std::array<uint8_t,3>& new_target)
    {
        start_val  = new_start;
        target_val = new_target;
        reset();
    }

    /** Reset with entirely new delay, start, and target. */
    void reset(uint32_t new_delay,
               const std::array<uint8_t,3>& new_start,
               const std::array<uint8_t,3>& new_target)
    {
        delay_ms   = new_delay;
        start_val  = new_start;
        target_val = new_target;
        reset();
    }
};

#endif  // ASYNCTIMERARRAY_H
