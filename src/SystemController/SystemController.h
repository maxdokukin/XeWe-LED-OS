/*********************************************************************************
 *  SPDX-License-Identifier: LicenseRef-PolyForm-NC-1.0.0-NoAI
 *
 *  Licensed under PolyForm Noncommercial 1.0.0 + No AI Use Addendum v1.0.
 *  See: LICENSE and LICENSE-NO-AI.md in the project root for full terms.
 *
 *  Required Notice: Copyright 2025 Maxim Dokukin (https://maxdokukin.com)
 *  https://github.com/maxdokukin/xewe-led-os
 *********************************************************************************/
#pragma once

#define INTERFACE_COUNT 5

#include "../Modules/Module/Module.h"

#include "../Modules/Software/SerialPort/SerialPort.h"
#include "../Modules/Software/System/System.h"
#include "../Modules/Software/CommandParser/CommandParser.h"
#include "../Modules/Hardware/Pins/Pins.h"
#include "../Modules/Hardware/Buttons/Buttons.h"
#include "../Modules/Software/Wifi/Wifi.h"
#include "../Modules/Software/WebInterface/WebInterface.h"

#include "../Interfaces/Software/Nvs/Nvs.h"
#include "../Interfaces/Hardware/LedStrip/LedStrip.h"

#include <array>
#include <vector>

class SystemController {
public:
    SystemController();

    void                        begin();
    void                        loop();

    void                        sync_color                  (const std::array<uint8_t,3> color,
                                                             const std::array<uint8_t,INTERFACE_COUNT>& sync_flags);
    void                        sync_brightness             (const uint8_t brightness,
                                                             const std::array<uint8_t,INTERFACE_COUNT>& sync_flags);
    void                        sync_state                  (const uint8_t state,
                                                             const std::array<uint8_t,INTERFACE_COUNT>& sync_flags);
    void                        sync_mode                   (const uint8_t mode,
                                                             const std::array<uint8_t,INTERFACE_COUNT>& sync_flags);
    void                        sync_length                 (const uint16_t length,
                                                             const std::array<uint8_t,INTERFACE_COUNT>& sync_flags);
    void                        sync_all                    (const std::array<uint8_t,3> color,
                                                             const uint8_t brightness,
                                                             const uint8_t state,
                                                             const uint8_t mode,
                                                             const uint16_t length,
                                                             const std::array<uint8_t,INTERFACE_COUNT>& sync_flags);

    SerialPort                  serial_port;
    Nvs                         nvs;
    System                      system;
    CommandParser               command_parser;
    Pins                        pins;
    Buttons                     buttons;
    Wifi                        wifi;
    WebInterface                web_interface;

    LedStrip                    led_strip;

    vector<Module*>&            get_modules                 () { return modules; }

private:
    template <typename Fn>
    void                        for_each_interface          (const std::array<uint8_t,INTERFACE_COUNT>& sync_flags, Fn&& fn);
    vector<Module*>             modules                     {};
    vector<Interface*>          interfaces                  {};
};

template <typename Fn>
void SystemController::for_each_interface(
    const std::array<uint8_t, INTERFACE_COUNT>& flags, Fn&& fn) {
    for (std::size_t i = 0; i < INTERFACE_COUNT; ++i) {

        if (i > 2) return;

        if (flags[i] && interfaces[i]) {
            std::forward<Fn>(fn)(*interfaces[i]);
        }
    }
}