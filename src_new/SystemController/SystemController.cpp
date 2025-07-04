#include "SystemController.h"

static Ticker ram_print_ticker;

SystemController::SystemController()
    : nvs(*this),
      webinterface(*this),
      alexa(*this),
      homekit(*this),
      buttons(*this), // NEW: Initialize button controller with a reference to this
      webinterface_module_active(false),
      alexa_module_active(false),
      homekit_module_active(false),
      buttons_module_active(false) // NEW: Initialize the button module flag
{}

// main functions
bool SystemController::begin() {
    DBG_PRINTF(SystemController, "SystemController::begin()\n");

    if (!serial_port_begin()) {
        system_restart(1000);
    }
    if (!nvs_begin()) {
        serial_port.println("NVS Init Failed!");
        system_restart(1000);
    }

    bool first_init_flag = nvs.read_uint8("first_init_done") == 0;

    if (!system_begin(first_init_flag)) {
        serial_port.println("System Init Failed!");
        system_restart(1000);
    }
    if (!led_strip_begin(first_init_flag)) {
        serial_port.println("Led Strip Init Failed!");
        system_restart(1000);
    }
    // NEW: Add button module to startup sequence
    if (!buttons_begin(first_init_flag)) {
        serial_port.println("Buttons Init Failed!");
        system_restart(1000);
    }
    if (first_init_flag || wifi_module_active) {
        if (!wifi_begin(first_init_flag)) {
            serial_port.println("WiFi Init Failed!");
            system_restart(1000);
        }
        if (!web_server_begin(first_init_flag)) {
            serial_port.println("Web Server Init Failed!");
            system_restart(1000);
        }
        if (!webinterface_begin(first_init_flag)) {
            serial_port.println("Web Interface Init Failed!");
            system_restart(1000);
        }
        if (!alexa_begin(first_init_flag)) {
            serial_port.println("Alexa Init Failed!");
            system_restart(1000);
        }
        if (!homekit_begin(first_init_flag)) {
            serial_port.println("HomeKit Init Failed!");
            system_restart(1000);
        }
    } else {
        serial_port.print("\n+------------------------------------------------+\n"
                          "|                 WiFi Disabled                  |\n"
                          "+------------------------------------------------+\n");
        serial_port.println("Skipping WiFi, Web Interface, Alexa, HomeKit inits\nType $wifi enable\nIf you'd like to use them");
    }
    if (!command_parser_begin(first_init_flag)) {
        serial_port.println("CMD Parser Init Failed!");
        system_restart(1000);
    }

    if (first_init_flag) {
        nvs.write_uint8("first_init_done", 1);
        nvs.commit();
        serial_port.print("\n+------------------------------------------------+\n"
                          "|              Initial setup success!            |\n"
                          "+------------------------------------------------+\n");
        serial_port.get_string("Press enter to restart");
        system_restart(1000);
    }

    system_sync_state("all", {true, true, true, true});

    return true;
}

void SystemController::loop() {
    nvs.loop();

    if (serial_port.has_line()) {
        String line = serial_port.read_line();
        command_parser.loop(line);
    }

    // NEW: Call the button controller's loop
    if (buttons_module_active) {
        buttons.loop();
    }

    if (wifi_module_active && wifi.is_connected()) {
        if (homekit_module_active)
            homekit.loop();
        if (alexa_module_active)
            alexa.loop();
        if (webinterface_module_active) {
            // alexa does it if active
            if(!alexa_module_active) {
                web_server.handleClient();
            }
            webinterface.loop();
        }
    }

    led_strip.loop();
}

// begin functions
bool SystemController::serial_port_begin    () {
    return serial_port.begin(115200);

}

bool SystemController::nvs_begin         () {
    nvs.begin((void*)"lsys_store");
    return nvs.is_initialized();
}

bool SystemController::system_begin         (bool first_init_flag){
    DBG_PRINTLN(SystemController, "system_begin()");
    serial_port.print("\n\n\n+------------------------------------------------+\n"
                         "|                   XeWe LED OS                  |\n"
                         "+------------------------------------------------+\n"
                         "|                   Version 2.0                  |\n"
                         "|    https://github.com/maxdokukin/XeWe-LED-OS   |\n"
                         "+------------------------------------------------+\n"
                         "|              ESP32 OS to control               |\n"
                         "|             addressable LED lights             |\n"
                         "+------------------------------------------------+\n"
                         "|            Communication supported:            |\n"
                         "|                                                |\n"
                         "|                      Alexa                     |\n"
                         "|                     HomeKit                    |\n"
                         "|                   Web Browser                  |\n"
                         "|                 Serial Port CLI                |\n"
                         "|                Physical Buttons                |\n" // NEW
                         "+------------------------------------------------+\n");

    if (first_init_flag) {
        serial_port.print("\n+------------------------------------------------+\n"
                          "|       Alright lets set things up for you       |\n"
                          "+------------------------------------------------+\n");


        serial_port.print("+------------------------------------------------+\n"
                          "|                   Set up flow                  |\n"
                          "|                                                |\n"
                          "|    - Device Name                               |\n"
                          "|    - LED Strip                                 |\n"
                          "|    - Buttons (Optional)                        |\n" // NEW
                          "|    - WiFi                                      |\n"
                          "|    - Web Interface           REQUIRES WiFi     |\n"
                          "|    - Alexa                   REQUIRES WiFi     |\n"
                          "|    - HomeKit                 REQUIRES WiFi     |\n"
                          "+------------------------------------------------+\n");

        serial_port.print("\n+------------------------------------------------+\n"
                          "|                 Name Your Device               |\n"
                          "+------------------------------------------------+\n");
        serial_port.println("Set the name your device will proudly hold until\nthe last electron leaves it\nSample names: \"Desk Lights\" or \"Ceiling Lights\"\n");
        String device_name;
        bool confirmed = false;
        while (!confirmed) {
            device_name = serial_port.get_string("Enter device name: ");
            confirmed = serial_port.prompt_user_yn("Confirm name: " + device_name);
        }
        nvs.write_str("device_name", device_name);
        serial_port.get_string("\nDevice name setup success!\n"
                               "Press enter to continue");
    }
    else {
        wifi_module_active         = nvs.read_bool("wifi_mod_act");
        webinterface_module_active = nvs.read_bool("webint_mod_act");
        alexa_module_active        = nvs.read_bool("alexa_mod_act");
        homekit_module_active      = nvs.read_bool("homekit_mod_act");
        buttons_module_active       = nvs.read_bool("buttons_mod_act");
        system_status();
    }
    return true;
}

bool SystemController::led_strip_begin      (bool first_init_flag) {
    serial_port.print("\n+------------------------------------------------+\n"
                      "|                 LED Strip Init                 |\n"
                      "+------------------------------------------------+\n");

    static CRGB main_leds[LED_STRIP_NUM_LEDS_MAX];
    FastLED.addLeds<LED_STRIP_TYPE, PIN_LED_STRIP, LED_STRIP_COLOR_ORDER>(main_leds, LED_STRIP_NUM_LEDS_MAX).setCorrection( TypicalLEDStrip );
    led_strip.begin(main_leds, LED_STRIP_NUM_LEDS_MAX);

    if (first_init_flag) {
        int led_num_entry = serial_port.get_int("How many LEDs do you have connected?\nEnter a number: ");
        serial_port.println("");

        while (true) {
            if (led_num_entry < 0) {
                serial_port.println("LED number must be greater than 0");
            } else if (led_num_entry > LED_STRIP_NUM_LEDS_MAX) {
                serial_port.println("That's too many. Max supported LED: " + String(LED_STRIP_NUM_LEDS_MAX));
            } else if (led_num_entry <= LED_STRIP_NUM_LEDS_MAX){
                nvs.write_uint16("led_len", led_num_entry);
                led_strip_reset(led_num_entry);
                break;
            }
        }

        serial_port.get_string("\nLED strip was set to green\n"
                               "If you don't see the green color check the\npin (GPIO), led type, and color order\n\n"
                               "LED setup success!\n"
                               "Press enter to continue");
    } else {
        led_strip_set_length        (nvs.read_uint16 ("led_len"),       {false, false, false, false});
        led_strip_set_state         (nvs.read_uint8 ("led_state"),      {false, false, false, false});
        led_strip_set_mode          (nvs.read_uint8("led_mode"),        {false, false, false, false});
        led_strip_set_rgb           ({nvs.read_uint8 ("led_r"),
                                      nvs.read_uint8 ("led_g"),
                                      nvs.read_uint8 ("led_b")},        {false, false, false, false});
        led_strip_set_brightness    (nvs.read_uint8 ("led_bri"),        {false, false, false, false});

        uint32_t start_time = millis();
        while(millis() - start_time < 1000)
            led_strip.loop();

        led_strip_status();
        serial_port.println("\nLED setup success!");
    }
    return true;
}

bool SystemController::buttons_begin(bool first_init_flag) {
    serial_port.print("\n+------------------------------------------------+\n"
                      "|                   Buttons Init                 |\n"
                      "+------------------------------------------------+\n");

    if (first_init_flag) {
        buttons_enable(false, false);
    } else if (buttons_module_active) {
        int btn_count = nvs.read_uint8("btn_count", 0);
        std::vector<String> configs;
        for (int i = 0; i < btn_count; i++) {
            String key = "btn_cfg_" + String(i);
            configs.push_back(nvs.read_str(key.c_str()));
        }
        buttons.load_configs(configs); // Pass the loaded configs
        serial_port.println("Buttons module enabled and " + String(btn_count) + " configurations loaded.");
        if (!btn_count) {
            serial_port.println("Use $buttons add");
        }
    } else {
        serial_port.println("Buttons Module is disabled.");
        serial_port.println("You can enable it using '$buttons enable'");
    }
    return true;
}

bool SystemController::wifi_begin           (bool first_init_flag) {
    serial_port.print("\n+------------------------------------------------+\n"
                      "|                    WiFi Init                   |\n"
                      "+------------------------------------------------+\n");

    if (first_init_flag){
        wifi_reset(false);
        wifi_enable(false, false);
        return true;
    }

    if (wifi_module_active) {
        wifi_connect(false);
    } else {
        serial_port.println("\nWiFi setup skipped");
        serial_port.println("You can enable WiFi using\n   $wifi enable");
    }
    return true;
}

bool SystemController::web_server_begin     (bool first_init_flag) {

    if (first_init_flag) {
        return true;
    }

    if (wifi_module_active) {
        return true;
    }

    serial_port.println("Web Server requires WiFi");
    return false;
}

bool SystemController::webinterface_begin  (bool first_init_flag) {
    serial_port.print("\n+------------------------------------------------+\n"
                      "|               Web Interface Init               |\n"
                      "+------------------------------------------------+\n");

    if (wifi_module_active) {
        if (first_init_flag){
            webinterface_enable(false, false);
            return true;
        }

        if (webinterface_module_active) {
            // alexa does it if active
            if(!alexa_module_active) {
                web_server.begin();
            }
            webinterface.begin((void*)&web_server, nvs.read_str("device_name"));

            serial_port.println(String("Web Interface setup success!\n") +
                                       "\nTo control LED from the browser, make sure that\n" +
                                       "the device (phone/laptop) connected to the same\nWiFi: " + wifi.get_ssid() + "\n" +
                                       "\nOpen in browser:\n" +
                                       "http://" + wifi.get_local_ip());
        } else {
            serial_port.println("Web Interface setup skipped");
            serial_port.println("Web Interface module disabled");
            serial_port.println("You can enable Web Interface using\n   $webinterface enable");
        }
    } else {
        serial_port.println("Web Interface setup skipped");
        serial_port.println("Web Interface requires WiFi");
    }
    return true;
}

bool SystemController::alexa_begin          (bool first_init_flag) {
    serial_port.print("\n+------------------------------------------------+\n"
                      "|                   Alexa Init                   |\n"
                      "+------------------------------------------------+\n");
    if (wifi_module_active) {
        if (first_init_flag){
            alexa_enable(false, false);
            return true;
        }

        if (alexa_module_active) {
            alexa.begin((void*)&web_server, nvs.read_str("device_name"));

            web_server.onNotFound([this]() {
                if (!alexa.get_instance().handleAlexaApiCall(web_server.uri(), web_server.arg("plain"))) {
                    web_server.send(404, "text/plain", "Endpoint not found.");
                }
            });

            serial_port.println("Alexa setup success!");
            serial_port.println("\nTo control LED with Alexa, make sure that");
            serial_port.println("Alexa is connected to the same\nWiFi: " + wifi.get_ssid());
            serial_port.println("\nAsk Alexa to discover devices");
        } else {
            serial_port.println("Alexa setup skipped");
            serial_port.println("Alexa module disabled");
            serial_port.println("You can enable Alexa using\n    $alexa enable");
        }
    } else {
        serial_port.println("Alexa setup skipped");
        serial_port.println("Alexa requires WiFi");
    }
    return true;
}

bool SystemController::homekit_begin        (bool first_init_flag) {
    serial_port.print("\n+------------------------------------------------+\n"
                      "|                  HomeKit Init                  |\n"
                      "+------------------------------------------------+\n");
    if (wifi_module_active) {
        if (first_init_flag){
            homekit_enable(false, false);
            return true;
        }

        if (homekit_module_active) {
            homekit.begin(nullptr, nvs.read_str("device_name"));

            uint32_t timestamp = millis();
            while(millis() - timestamp < 2000)
                homekit.loop();

            serial_port.println("HomeKit setup success!");
            serial_port.println("\nTo control LED with Home App on iPhone/iPad/Mac, ");
            serial_port.println("make sure that device is connected to the same\nWiFi: " + wifi.get_ssid());

            serial_port.println("\nScan this QR code:\nhttps://github.com/maxdokukin/XeWe-LedOS/blob/main/doc/HomeKit_Connect_QR.png");
            serial_port.println("Or go to Home App\nPress add device, using code 4663-7726");
        } else {
            serial_port.println("HomeKit setup skipped");
            serial_port.println("HomeKit module disabled");
            serial_port.println("You can enable HomeKit using\n$homekit enable");
        }
    } else {
        serial_port.println("HomeKit setup skipped");
        serial_port.println("HomeKit requires WiFi");
    }
    return true;
}

bool SystemController::command_parser_begin (bool first_init_flag) {
    DBG_PRINTLN(SystemController, "command_parser_begin()");

    if (first_init_flag)
        return true;

    serial_port.print("\n+------------------------------------------------+\n"
                      "|           Command Line Interface Init          |\n"
                      "+------------------------------------------------+\n"
                      "|     Use $help to see all available commands    |\n"
                      "+------------------------------------------------+\n");
    // $help
    command_parser_commands[0] =    { "",               "Print all cmd available",              0, [this](auto&){ command_parser.print_all_commands(); } };

    // $system
    system_commands[0] =            { "help",           "Show this help message",               0, [this](auto&){ command_parser.print_help("system"); } };
    system_commands[1] =            { "reset",          "Reset everything (will restart)",      0, [this](auto&){ system_reset(); } };
    system_commands[2] =            { "status",         "Get web system status",                0, [this](auto&){ system_status(); } };
    system_commands[3] =            { "restart",        "Restart system",                       0, [this](auto&){ system_restart(0); } };

    // $led
    led_strip_commands[0]  =        { "help",           "Show this help message",               0, [this](auto&){ command_parser.print_help("led"); } };
    led_strip_commands[1]  =        { "reset",          "Clear stored led data",                0, [this](auto&){ led_strip_reset(); } };
    led_strip_commands[2]  =        { "status",         "Clear stored led data",                0, [this](auto&){ led_strip_status(); } };
    led_strip_commands[3]  =        { "set_mode",       "Set LED strip mode",                   1, [this](auto& a){ led_strip_set_mode(a); } };
    led_strip_commands[4]  =        { "set_rgb",        "Set RGB color",                        3, [this](auto& a){ led_strip_set_rgb(a); } };
    led_strip_commands[5]  =        { "set_r",          "Set red channel",                      1, [this](auto& a){ led_strip_set_r(a); } };
    led_strip_commands[6]  =        { "set_g",          "Set green channel",                    1, [this](auto& a){ led_strip_set_g(a); } };
    led_strip_commands[7]  =        { "set_b",          "Set blue channel",                     1, [this](auto& a){ led_strip_set_b(a); } };
    led_strip_commands[8]  =        { "set_hsv",        "Set HSV color",                        3, [this](auto& a){ led_strip_set_hsv(a); } };
    led_strip_commands[9]  =        { "set_hue",        "Set hue channel",                      1, [this](auto& a){ led_strip_set_hue(a); } };
    led_strip_commands[10] =        { "set_sat",        "Set saturation channel",               1, [this](auto& a){ led_strip_set_sat(a); } };
    led_strip_commands[11] =        { "set_val",        "Set value channel",                    1, [this](auto& a){ led_strip_set_val(a); } };
    led_strip_commands[12] =        { "set_brightness", "Set global brightness",                1, [this](auto& a){ led_strip_set_brightness(a); } };
    led_strip_commands[13] =        { "set_state",      "Set on/off state",                     1, [this](auto& a){ led_strip_set_state(a); } };
    led_strip_commands[14] =        { "toggle_state",   "If off->on, if on->off",               0, [this](auto&){ led_strip_toggle_state(); } };
    led_strip_commands[15] =        { "turn_on",        "Turn strip on",                        0, [this](auto&){ led_strip_turn_on(); } };
    led_strip_commands[16] =        { "turn_off",       "Turn strip off",                       0, [this](auto&){ led_strip_turn_off(); } };
    led_strip_commands[17] =        { "set_length",     "Set new number of LEDs",               1, [this](auto& a){ led_strip_set_length(a); } };

    // $wifi
    wifi_commands[0] =              { "help",           "Show this help message",               0, [this](auto&){ command_parser.print_help("wifi"); } };
    wifi_commands[1] =              { "reset",          "Reset web interface",                  0, [this](auto&){ wifi_reset(true); } };
    wifi_commands[2] =              { "status",         "Get web interface status",             0, [this](auto&){ wifi_status(); } };
    wifi_commands[3] =              { "enable",         "Enable web interface",                 0, [this](auto&){ wifi_enable(false, true); } };
    wifi_commands[4] =              { "disable",        "Disable web interface",                0, [this](auto&){ wifi_disable(false, true); } };
    wifi_commands[5] =              { "connect",        "Connect or reconnect to WiFi",         0, [this](auto&){ wifi_connect(true); } };
    wifi_commands[6] =              { "disconnect",     "Disconnect from WiFi",                 0, [this](auto&){ wifi_disconnect(); } };
    wifi_commands[7] =              { "scan",           "List available WiFi networks",         0, [this](auto&){ wifi_get_available_networks(); } };

    // $webinterface
    webinterface_commands[0] =      { "help",           "Show this help message",               0, [this](auto&){ command_parser.print_help("webinterface"); } };
    webinterface_commands[1] =      { "reset",          "Reset web interface",                  0, [this](auto&){ webinterface_reset(); } };
    webinterface_commands[2] =      { "status",         "Get web interface status",             0, [this](auto&){ webinterface_status(); } };
    webinterface_commands[3] =      { "enable",         "Enable web interface",                 0, [this](auto&){ webinterface_enable(false, true); } };
    webinterface_commands[4] =      { "disable",        "Disable web interface",                0, [this](auto&){ webinterface_disable(false, true); } };

    // $alexa
    alexa_commands[0] =             { "help",           "Show this help message",               0, [this](auto&){ command_parser.print_help("alexa"); } };
    alexa_commands[1] =             { "reset",          "Reset Alexa integration",              0, [this](auto&){ alexa_reset(); } };
    alexa_commands[2] =             { "status",         "Get Alexa status",                     0, [this](auto&){ alexa_status(); } };
    alexa_commands[3] =             { "enable",         "Enable Alexa integration",             0, [this](auto&){ alexa_enable(false, true); } };
    alexa_commands[4] =             { "disable",        "Disable Alexa integration",            0, [this](auto&){ alexa_disable(false, true); } };

    // $homekit
    homekit_commands[0] =           { "help",           "Show this help message",               0, [this](auto&){ command_parser.print_help("homekit"); } };
    homekit_commands[1] =           { "reset",          "Reset HomeKit integration",            0, [this](auto&){ homekit_reset(); } };
    homekit_commands[2] =           { "status",         "Get HomeKit status",                   0, [this](auto&){ homekit_status(); } };
    homekit_commands[3] =           { "enable",         "Enable HomeKit integration",           0, [this](auto&){ homekit_enable(false, true); } };
    homekit_commands[4] =           { "disable",        "Disable HomeKit integration",          0, [this](auto&){ homekit_disable(false, true); } };

    // $ram
    ram_commands[0] =               { "help",           "Show this help message",               0, [this](auto&){ command_parser.print_help("ram"); } };
    ram_commands[1] =               { "status",         "Show overall heap stats",              0, [this](auto&){ ram_status(); } };
    ram_commands[2] =               { "free",           "Print current free heap bytes",        0, [this](auto&){ ram_free(); } };
    ram_commands[3] =               { "watch",          "Print free heap every <ms>",           1, [this](auto& a){ ram_watch(a); } };

    // NEW: Define button commands
    buttons_commands[0] =           { "help",           "Show this help message",               0, [this](auto&){ command_parser.print_help("buttons"); } };
    buttons_commands[1] =           { "reset",          "Reset and clear all button actions",   0, [this](auto&){ buttons_reset(); } };
    buttons_commands[2] =           { "status",         "Get configured buttons status",        0, [this](auto&){ buttons_status(); } };
    buttons_commands[3] =           { "enable",         "Enable button integration",            0, [this](auto&){ buttons_enable(false, true); } };
    buttons_commands[4] =           { "disable",        "Disable button integration",           0, [this](auto&){ buttons_disable(false, true); } };
    // $buttons add 9 "$led turn_off" pullup on_press 50    //
    buttons_commands[5] =           { "add",            "Add cmd to run on button event",       5, [this](auto& a){ buttons_add(a); } };
    buttons_commands[6] =           { "remove",         "Remove button action by pin",          1, [this](auto& a){ buttons_remove(a); } };

    // NEW: Register command groups including buttons
    command_groups[0] =             { "help",           command_parser_commands,                COMMAND_PARSER_CMD_COUNT };
    command_groups[1] =             { "system",         system_commands,                        SYSTEM_CMD_COUNT    };
    command_groups[2] =             { "led",            led_strip_commands,                     LED_STRIP_CMD_COUNT };
    command_groups[3] =             { "wifi",           wifi_commands,                          WIFI_CMD_COUNT      };
    command_groups[4] =             { "webinterface",   webinterface_commands,                  WEBINTERFACE_CMD_COUNT };
    command_groups[5] =             { "alexa",          alexa_commands,                         ALEXA_CMD_COUNT     };
    command_groups[6] =             { "homekit",        homekit_commands,                       HOMEKIT_CMD_COUNT   };
    command_groups[7] =             { "ram",            ram_commands,                           RAM_CMD_COUNT       };
    command_groups[8] =             { "buttons",         buttons_commands,                      BUTTONS_CMD_COUNT    };

    command_parser.begin(command_groups, CMD_GROUP_COUNT);

    return true;
}


// --- SYSTEM ---

void SystemController::system_status() {
    serial_port.print(String("\n+------------------------------------------------+\n") +
                      "|              System Configuration              |\n" +
                      "|                                                |\n" +
                      "| Buttons        : " + (buttons_module_active ? "enabled " : "disabled") + "                      |\n" +
                      "| WiFi           : " + (wifi_module_active ? "enabled " : "disabled") + "                      |\n" +
                      "| Web Interface  : " + (webinterface_module_active ? "enabled " : "disabled") + "                      |\n" +
                      "| Alexa          : " + (alexa_module_active ? "enabled " : "disabled") + "                      |\n" +
                      "| HomeKit        : " + (homekit_module_active ? "enabled " : "disabled") + "                      |\n" +
                      "+------------------------------------------------+\n");
}

void SystemController::system_reset(){
    DBG_PRINTLN(SystemController, "system_reset()");
    serial_port.println("\n+------------------------------------------------+\n"
                        "|                  Resetting...                  |\n"
                        "+------------------------------------------------+\n");
    nvs.reset();
    serial_port.println("NOTE: You need to manually remove device from\nAlexa and Apple Home Apps!");
    serial_port.get_string("System reset success!\n\nPress enter to restart");
    system_restart(1000);
}

void SystemController::system_restart(uint16_t delay_before){
    DBG_PRINTLN(SystemController, "system_restart()");
    serial_port.println("\n+------------------------------------------------+\n"
                        "|                 Restarting...                  |\n"
                        "+------------------------------------------------+\n");
    nvs.commit();
    delay(delay_before);
    ESP.restart();
}

//todo
//this method can use enum for efficiency
void SystemController::system_sync_state(String field, std::array<bool, 4> sync_flags) {
    // --- ADDED DEBUG ---
    // Print the incoming field and the status of the sync flags for each module.
    DBG_PRINTF(SystemController, "system_sync_state: field='%s', flags=[Mem:%d, Web:%d, Alexa:%d, HK:%d]\n",
        field.c_str(), sync_flags[0], sync_flags[1], sync_flags[2], sync_flags[3]);
    // -------------------

    if (strcmp(field.c_str(), "color") == 0) {
        std::array<uint8_t, 3> rgb = led_strip.get_target_rgb();
        std::array<uint8_t, 3> hsv = led_strip.get_target_hsv();

        // --- ADDED DEBUG ---
        DBG_PRINTF(SystemController, " -> Syncing color: RGB=(%u,%u,%u), HSV=(%u,%u,%u)\n", rgb[0], rgb[1], rgb[2], hsv[0], hsv[1], hsv[2]);
        // -------------------

        if (sync_flags[0])                                    nvs.sync_color                 (rgb);
        if (sync_flags[1] && webinterface_module_active)      webinterface.sync_color          (rgb);
        if (sync_flags[2] && alexa_module_active)             alexa.sync_color                  (rgb);
        if (sync_flags[3] && homekit_module_active)           homekit.sync_color                (hsv);

    } else if (strcmp(field.c_str(), "brightness") == 0) {
        uint8_t target_brightness = led_strip.get_target_brightness();

        // --- ADDED DEBUG ---
        DBG_PRINTF(SystemController, " -> Syncing brightness: %u\n", target_brightness);
        // -------------------

        if (sync_flags[0])                                    nvs.sync_brightness          (target_brightness);
        if (sync_flags[1] && webinterface_module_active)      webinterface.sync_brightness   (target_brightness);
        if (sync_flags[2] && alexa_module_active)             alexa.sync_brightness           (target_brightness);
        if (sync_flags[3] && homekit_module_active)           homekit.sync_brightness         (target_brightness);

    } else if (strcmp(field.c_str(), "state") == 0) {
        bool target_state = led_strip.get_target_state();

        // --- ADDED DEBUG ---
        DBG_PRINTF(SystemController, " -> Syncing state: %s\n", target_state ? "ON" : "OFF");
        // -------------------

        if (sync_flags[0])                                    nvs.sync_state               (target_state);
        if (sync_flags[1] && webinterface_module_active)      webinterface.sync_state        (target_state);
        if (sync_flags[2] && alexa_module_active)             alexa.sync_state                (target_state);
        if (sync_flags[3] && homekit_module_active)           homekit.sync_state              (target_state);

    } else if (strcmp(field.c_str(), "mode") == 0) {
        uint8_t target_mode_id = led_strip.get_target_mode_id();
        String target_mode_name = led_strip.get_target_mode_name();

        // --- ADDED DEBUG ---
        DBG_PRINTF(SystemController, " -> Syncing mode: ID=%u, Name='%s'\n", target_mode_id, target_mode_name.c_str());
        // -------------------

        if (sync_flags[0])                                    nvs.sync_mode                (target_mode_id, target_mode_name);
        if (sync_flags[1] && webinterface_module_active)      webinterface.sync_mode         (target_mode_id, target_mode_name);
        if (sync_flags[2] && alexa_module_active)             alexa.sync_mode                 (target_mode_id, target_mode_name);
        if (sync_flags[3] && homekit_module_active)           homekit.sync_mode               (target_mode_id, target_mode_name);

    } else if (strcmp(field.c_str(), "length") == 0) {
        uint16_t length = led_strip.get_length();

        // --- ADDED DEBUG ---
        DBG_PRINTF(SystemController, " -> Syncing length: %u\n", length);
        // -------------------

        if (sync_flags[0])                                    nvs.sync_length              (length);
        if (sync_flags[1] && webinterface_module_active)      webinterface.sync_length       (length);
        if (sync_flags[2] && alexa_module_active)             alexa.sync_length               (length);
        if (sync_flags[3] && homekit_module_active)           homekit.sync_length             (length);

    } else if (strcmp(field.c_str(), "all") == 0) {
        std::array<uint8_t, 3>                                  target_rgb                      = led_strip.get_target_rgb();
        std::array<uint8_t, 3>                                  target_hsv                      = led_strip.get_target_hsv();
        uint8_t                                                 target_brightness               = led_strip.get_target_brightness();
        bool                                                    target_state                    = led_strip.get_target_state();
        uint8_t                                                 target_mode_id                  = led_strip.get_target_mode_id();
        String                                                  target_mode_name                = led_strip.get_target_mode_name();
        uint16_t                                                length                          = led_strip.get_length();

        // --- ADDED DEBUG ---
        DBG_PRINTF(SystemController, " -> Syncing all: RGB=(%u,%u,%u), Bri=%u, State=%d, Mode=%u, Len=%u\n",
            target_rgb[0], target_rgb[1], target_rgb[2], target_brightness, target_state, target_mode_id, length);
        // -------------------

        if (sync_flags[0])                                    nvs.sync_all                 (target_rgb, target_brightness, target_state, target_mode_id, target_mode_name, length);
        if (sync_flags[1] && webinterface_module_active)      webinterface.sync_all          (target_rgb, target_brightness, target_state, target_mode_id, target_mode_name, length);
        if (sync_flags[2] && alexa_module_active)             alexa.sync_all                  (target_rgb, target_brightness, target_state, target_mode_id, target_mode_name, length);
        if (sync_flags[3] && homekit_module_active)           homekit.sync_all                (target_hsv, target_brightness, target_state, target_mode_id, target_mode_name, length);
    }
}


// --- LED handlers ---
void                            SystemController::led_strip_reset               (uint16_t led_num){
    DBG_PRINTLN(SystemController, "led_strip_reset()");
    led_strip_set_length        (led_num,      {true, true, true, true});
    led_strip_set_state         (1,            {true, true, true, true});
    led_strip_set_mode          (0,            {true, true, true, true});
    led_strip_set_rgb           ({0, 255,  0}, {true, true, true, true});
    led_strip_set_brightness    (10,           {true, true, true, true});
    led_strip_status();

    uint32_t start_time = millis();
    while(millis() - start_time < 1000)
        led_strip.loop();
}

void                            SystemController::led_strip_status              () {
    DBG_PRINTLN(SystemController, "led_strip_status()");
    serial_port.println("LED Strip Config:");
    serial_port.println("    Pin          : GPIO" + String(PIN_LED_STRIP));
    serial_port.println("    Type         : " + String(TO_STRING(LED_STRIP_TYPE)));
    serial_port.println("    Color order  : " + String(TO_STRING(LED_STRIP_COLOR_ORDER)));
    serial_port.println("    Max LED      : " + String(LED_STRIP_NUM_LEDS_MAX));
    serial_port.println("    These can only be changed in the src/Config.h\n");
    serial_port.println("    Length       : " + String(led_strip.get_length()));
    serial_port.println("    State        : " + String(led_strip.get_state() ? "ON" : "OFF"));
    serial_port.println("    Brightness   : " + String(led_strip.get_brightness()));
    serial_port.println("    Mode         : " + led_strip.get_mode_name());
    serial_port.println("    R            : " + String(led_strip.get_r()));
    serial_port.println("    G            : " + String(led_strip.get_g()));
    serial_port.println("    B            : " + String(led_strip.get_b()));
}

void                            SystemController::led_strip_set_mode            (const String& args) {
    DBG_PRINTF(SystemController, "led_strip_set_mode(args=\"%s\")\n", args.c_str());
    uint8_t new_mode = static_cast<uint8_t>(args.toInt());
    led_strip_set_mode(new_mode, {true, true, true, true});
}

void                            SystemController::led_strip_set_mode            (uint8_t new_mode, std::array<bool, 4> sync_flags) {
    DBG_PRINTF(SystemController, "led_strip_set_mode(new_mode=%u, sync_flags=[%d,%d,%d, %d])\n", new_mode, sync_flags[0], sync_flags[1], sync_flags[2], sync_flags[3]);
    led_strip.set_mode(new_mode);
    system_sync_state("mode", sync_flags);
}

void                            SystemController::led_strip_set_rgb             (const String& args) {
    DBG_PRINTF(SystemController, "led_strip_set_rgb(args=\"%s\")\n", args.c_str());
    int i1 = args.indexOf(' '), i2 = args.indexOf(' ', i1 + 1);
    uint8_t new_r = args.substring(0, i1).toInt();
    uint8_t new_g = args.substring(i1 + 1, i2).toInt();
    uint8_t new_b = args.substring(i2 + 1).toInt();
    led_strip_set_rgb({new_r, new_g, new_b}, {true, true, true, true});
}

void                            SystemController::led_strip_set_rgb             (std::array<uint8_t, 3> new_rgb, std::array<bool, 4> sync_flags) {
    DBG_PRINTF(SystemController, "led_strip_set_rgb(new_rgb=[%u,%u,%u], sync_flags=[%d,%d,%d,%d])\n", new_rgb[0], new_rgb[1], new_rgb[2], sync_flags[0], sync_flags[1], sync_flags[2], sync_flags[3]);
    led_strip.set_rgb(new_rgb);
    system_sync_state("color", sync_flags);
}

void                            SystemController::led_strip_set_r               (const String& args) {
    DBG_PRINTF(SystemController, "led_strip_set_r(args=\"%s\")\n", args.c_str());
    uint8_t new_r = static_cast<uint8_t>(args.toInt());
    led_strip_set_r(new_r, {true, true, true, true});
}

void                            SystemController::led_strip_set_r               (uint8_t new_r, std::array<bool, 4> sync_flags) {
    DBG_PRINTF(SystemController, "led_strip_set_r(new_r=%u, sync_flags=[%d,%d,%d,%d])\n", new_r, sync_flags[0], sync_flags[1], sync_flags[2], sync_flags[3]);
    led_strip.set_r(new_r);
    system_sync_state("color", sync_flags);
}

void                            SystemController::led_strip_set_g               (const String& args) {
    DBG_PRINTF(SystemController, "led_strip_set_g(args=\"%s\")\n", args.c_str());
    uint8_t new_g = static_cast<uint8_t>(args.toInt());
    led_strip_set_g(new_g, {true, true, true, true});
}

void                            SystemController::led_strip_set_g               (uint8_t new_g, std::array<bool, 4> sync_flags) {
    DBG_PRINTF(SystemController, "led_strip_set_g(new_g=%u, sync_flags=[%d,%d,%d,%d])\n", new_g, sync_flags[0], sync_flags[1], sync_flags[2], sync_flags[3]);
    led_strip.set_g(new_g);
    system_sync_state("color", sync_flags);
}

void                            SystemController::led_strip_set_b               (const String& args) {
    DBG_PRINTF(SystemController, "led_strip_set_b(args=\"%s\")\n", args.c_str());
    uint8_t new_b = static_cast<uint8_t>(args.toInt());
    led_strip_set_b(new_b, {true, true, true, true});
}

void                            SystemController::led_strip_set_b               (uint8_t new_b, std::array<bool, 4> sync_flags) {
    DBG_PRINTF(SystemController, "led_strip_set_b(new_b=%u, sync_flags=[%d,%d,%d,%d])\n", new_b, sync_flags[0], sync_flags[1], sync_flags[2], sync_flags[3]);
    led_strip.set_b(new_b);
    system_sync_state("color", sync_flags);
}

void                            SystemController::led_strip_set_hsv             (const String& args) {
    DBG_PRINTF(SystemController, "led_strip_set_hsv(args=\"%s\")\n", args.c_str());
    int i1 = args.indexOf(' '), i2 = args.indexOf(' ', i1 + 1);
    uint8_t new_hue = args.substring(0, i1).toInt();
    uint8_t new_sat = args.substring(i1 + 1, i2).toInt();
    uint8_t new_val = args.substring(i2 + 1).toInt();
    led_strip_set_hsv({new_hue, new_sat, new_val}, {true, true, true, true});
}

void                            SystemController::led_strip_set_hsv             (std::array<uint8_t, 3> new_hsv, std::array<bool, 4> sync_flags) {
    DBG_PRINTF(SystemController, "led_strip_set_hsv(new_hsv=[%u,%u,%u], sync_flags=[%d,%d,%d,%d])\n", new_hsv[0], new_hsv[1], new_hsv[2], sync_flags[0], sync_flags[1], sync_flags[2], sync_flags[3]);
    led_strip.set_hsv(new_hsv);
    system_sync_state("color", sync_flags);
}

void                            SystemController::led_strip_set_hue             (const String& args) {
    DBG_PRINTF(SystemController, "led_strip_set_hue(args=\"%s\")\n", args.c_str());
    uint8_t new_hue = static_cast<uint8_t>(args.toInt());
    led_strip_set_hue(new_hue, {true, true, true, true});
}

void                            SystemController::led_strip_set_hue             (uint8_t new_hue, std::array<bool, 4> sync_flags) {
    DBG_PRINTF(SystemController, "led_strip_set_hue(new_hue=%u, sync_flags=[%d,%d,%d,%d])\n", new_hue, sync_flags[0], sync_flags[1], sync_flags[2], sync_flags[3]);
    led_strip.set_h(new_hue);
    system_sync_state("color", sync_flags);
}

void                            SystemController::led_strip_set_sat             (const String& args) {
    DBG_PRINTF(SystemController, "led_strip_set_sat(args=\"%s\")\n", args.c_str());
    uint8_t new_sat = static_cast<uint8_t>(args.toInt());
    led_strip_set_sat(new_sat, {true, true, true, true});
}

void                            SystemController::led_strip_set_sat             (uint8_t new_sat, std::array<bool, 4> sync_flags) {
    DBG_PRINTF(SystemController, "led_strip_set_sat(new_sat=%u, sync_flags=[%d,%d,%d,%d])\n", new_sat, sync_flags[0], sync_flags[1], sync_flags[2], sync_flags[3]);
    led_strip.set_s(new_sat);
    system_sync_state("color", sync_flags);
}

void                            SystemController::led_strip_set_val             (const String& args) {
    DBG_PRINTF(SystemController, "led_strip_set_val(args=\"%s\")\n", args.c_str());
    uint8_t new_val = static_cast<uint8_t>(args.toInt());
    led_strip_set_val(new_val, {true, true, true, true});
}

void                            SystemController::led_strip_set_val             (uint8_t new_val, std::array<bool, 4> sync_flags) {
    DBG_PRINTF(SystemController, "led_strip_set_val(new_val=%u, sync_flags=[%d,%d,%d,%d])\n", new_val, sync_flags[0], sync_flags[1], sync_flags[2], sync_flags[3]);
    led_strip.set_v(new_val);
    system_sync_state("color", sync_flags);
}

void                            SystemController::led_strip_set_brightness      (const String& args) {
    DBG_PRINTF(SystemController, "led_strip_set_brightness(args=\"%s\")\n", args.c_str());
    uint8_t new_brightness = static_cast<uint8_t>(args.toInt());
    led_strip_set_brightness(new_brightness, {true, true, true, true});

}

void                            SystemController::led_strip_set_brightness      (uint8_t new_brightness, std::array<bool, 4> sync_flags) {
    DBG_PRINTF(SystemController, "led_strip_set_brightness(new_brightness=%u, sync_flags=[%d,%d,%d,%d])\n", new_brightness, sync_flags[0], sync_flags[1], sync_flags[2], sync_flags[3]);
    led_strip.set_brightness(new_brightness);
    system_sync_state("brightness", sync_flags);
}

void                            SystemController::led_strip_set_state           (const String& args) {
    DBG_PRINTF(SystemController, "led_strip_set_state(args=\"%s\")\n", args.c_str());
    bool new_state = static_cast<bool>(args.toInt());
    led_strip_set_state(new_state, {true, true, true, true});
}

void                            SystemController::led_strip_set_state           (bool new_state, std::array<bool, 4> sync_flags) {
    DBG_PRINTF(SystemController, "led_strip_set_state(new_state=%d, sync_flags=[%d,%d,%d,%d])\n", new_state, sync_flags[0], sync_flags[1], sync_flags[2], sync_flags[3]);
    led_strip.set_state(new_state);
    system_sync_state("state", sync_flags);
}

void                            SystemController::led_strip_toggle_state        () {
    DBG_PRINTLN(SystemController, "led_strip_toggle_state()");
    led_strip_toggle_state({true, true, true, true});

}

void                            SystemController::led_strip_toggle_state        (std::array<bool, 4> sync_flags) {
    DBG_PRINTF(SystemController, "led_strip_toggle_state(sync_flags=[%d,%d,%d,%d])\n", sync_flags[0], sync_flags[1], sync_flags[2], sync_flags[3]);
    led_strip.toggle_state();
    system_sync_state("state", sync_flags);
}

void                            SystemController::led_strip_turn_on             () {
    DBG_PRINTLN(SystemController, "led_strip_turn_on()");
    led_strip_turn_on({true, true, true, true});
    
}

void                            SystemController::led_strip_turn_on             (std::array<bool, 4> sync_flags) {
    DBG_PRINTF(SystemController, "led_strip_turn_on(sync_flags=[%d,%d,%d,%d])\n", sync_flags[0], sync_flags[1], sync_flags[2], sync_flags[3]);
    led_strip.turn_on();
    system_sync_state("state", sync_flags);
}

void                            SystemController::led_strip_turn_off            () {
    DBG_PRINTLN(SystemController, "led_strip_turn_off()");
    led_strip_turn_off({true, true, true, true});

}

void                            SystemController::led_strip_turn_off            (std::array<bool, 4> sync_flags) {
    DBG_PRINTF(SystemController, "led_strip_turn_off(sync_flags=[%d,%d,%d,%d])\n", sync_flags[0], sync_flags[1], sync_flags[2], sync_flags[3]);
    led_strip.turn_off();
    system_sync_state("state", sync_flags);
}


void                            SystemController::led_strip_set_length          (const String& args) {
    DBG_PRINTF(SystemController, "led_strip_set_length(args=\"%s\")\n", args.c_str());
    uint16_t new_length = static_cast<uint16_t>(args.toInt());
    led_strip_set_length(new_length, {true, true, true, true});
}

void                            SystemController::led_strip_set_length          (uint16_t new_length, std::array<bool, 4> sync_flags) {
    DBG_PRINTF(SystemController, "led_strip_set_length(new_length=%u, sync_flags=[%d,%d,%d,%d])\n", new_length, sync_flags[0], sync_flags[1], sync_flags[2], sync_flags[3]);
    led_strip.set_length(new_length);
    system_sync_state("length", sync_flags);
}
//// getters
std::array<uint8_t, 3>          SystemController::led_strip_get_target_rgb        ()                      const {
    DBG_PRINTLN(SystemController, "led_get_target_rgb() const {");
    return led_strip.get_target_rgb();
}

std::array<uint8_t, 3>          SystemController::led_strip_get_target_hsv        ()                      const {
    DBG_PRINTLN(SystemController, "led_strip_get_target_hsv() const {");
    return led_strip.get_target_hsv();
}

uint8_t                         SystemController::led_strip_get_target_brightness        ()                      const {
    DBG_PRINTLN(SystemController, "uint8_t SystemController::led_strip_get_target_brightness() const {");
    return led_strip.get_target_brightness();
}

bool                            SystemController::led_strip_get_target_state             ()                      const {
    DBG_PRINTLN(SystemController, "bool SystemController::led_strip_get_target_state() const {");
    return led_strip.get_target_state();
}

uint8_t                         SystemController::led_strip_get_target_mode_id           ()                     const       {
    DBG_PRINTLN(SystemController, "uint8_t SystemController::led_strip_get_target_mode_id() const {");
    return led_strip.get_target_mode_id();
}

String                         SystemController::led_strip_get_target_mode_name           ()                    const        {
    DBG_PRINTLN(SystemController, "uint8_t SystemController::led_strip_get_target_mode_name() const {");
    return led_strip.get_target_mode_name();
}


//////WIFI/////
void SystemController::wifi_enable(bool force_enable, bool force_restart) {
    system_module_enable(
        wifi_module_active,
        "WiFi Module",
        "wifi_mod_act",
        "This allows LED control via Browser, Alexa,\nand Apple Home App (iPhone/iPad/Mac)",
        force_enable,
        true, // Dependency flag: Pass 'true' because it has no dependency that can fail
        nullptr, // Dependency error message
        force_restart,
        // Custom Action: Call wifi_connect() after enabling
        [this]() { this->serial_port.println(""); this->wifi_connect(true); },
        // Custom Message: Provide the unique "already enabled" text
        "WiFi Module already enabled\nUse $wifi reset and $wifi connect"
    );
}

void SystemController::wifi_disable(bool force_disable, bool force_restart) {
    system_module_disable(
        wifi_module_active,
        "WiFi Module",
        "wifi_mod_act",
        force_disable,
        force_restart,
        [this]() {
            this->wifi_reset(false);
            this->webinterface_disable(true, false);
            this->alexa_disable(true, false);
            this->homekit_disable(true, false);
        }
    );
}

bool SystemController::wifi_connect(bool prompt_for_credentials) {
    DBG_PRINTF(SystemController, "wifi_connect(prompt_for_credentials=%d)\n", prompt_for_credentials);
    if (!wifi_module_active) {
        serial_port.println("WiFi Module disabled\n Use $wifi enable");
        return false;
    }
    if (wifi.is_connected()) {
        serial_port.println("Already connected");
        wifi_status();
        serial_port.println("Use '$wifi reset' to change network");
        return true;
    }
    String ssid, pwd;
    if (wifi_read_stored_credentials(ssid, pwd)) {
        serial_port.println("Stored WiFi credentials found");
        if (wifi_join(ssid, pwd)) {
            return true;
        } else {
            serial_port.println("Stored WiFi credentials not valid.");
            if (!prompt_for_credentials) {
                serial_port.println("Use '$wifi reset' to reset credentials");
            }
        }
    } else {
        serial_port.println("Stored WiFi credentials not found");
        if (!prompt_for_credentials) {
            serial_port.println("Type '$wifi connect' to select a new network");
        }
    }
    if (!prompt_for_credentials) {
        return false;
    }
    while (!wifi.is_connected()) {
        uint8_t prompt_status = wifi_prompt_for_credentials(ssid, pwd);
        if (prompt_status == 2) {
            serial_port.println("Terminated WiFi setup");
            return false;
        } else if (prompt_status == 1) {
            serial_port.println("Invalid choice");
            continue;
        } else if (prompt_status == 0) {
            if (wifi_join(ssid, pwd)) {
                return true;
            } else {
                continue;
            }
        }
    }
    return false;
}

void SystemController::wifi_status() {
    DBG_PRINTLN(SystemController, "wifi_status()");
    if (!wifi_module_active) {
        serial_port.println("WiFi Module disabled\n Use $wifi enable");
        return;
    }
    if (!wifi.is_connected()) {
        serial_port.println("\nWiFi not connected");
        return;
    }
    serial_port.println("\nConnected to " + wifi.get_ssid());
    serial_port.println("Local IP: " + wifi.get_local_ip());
    serial_port.println("MAC: " + wifi.get_mac_address());
}

bool SystemController::wifi_read_stored_credentials(String& ssid, String& pwd) {
    DBG_PRINTLN(SystemController, "wifi_read_stored_credentials(...)");
    if (!wifi_module_active) {
        serial_port.println("WiFi Module disabled\n Use $wifi enable");
        return false;
    }
    if (!nvs.read_bool("wifi_setup")) {
        return false;
    }
    ssid = nvs.read_str("wifi_name");
    pwd  = nvs.read_str("wifi_pass");
    return true;
}

uint8_t SystemController::wifi_prompt_for_credentials(String& ssid, String& pwd) {
    DBG_PRINTLN(SystemController, "wifi_prompt_for_credentials(...)");
    if (!wifi_module_active) {
        serial_port.println("WiFi Module disabled\n Use $wifi enable");
        return 2;
    }
    nvs.write_bool("wifi_setup", false);

    std::vector<String> networks = wifi_get_available_networks();
    int choice = serial_port.get_int("\nSelect network by number, or enter -1 to exit: ");
    if (choice == -1) {
        return 2;
    } else if (choice >= 0 && choice < (int)networks.size()) {
        ssid = networks[choice];
    } else {
        return 1;
    }
    pwd = serial_port.get_string("Selected: '" + ssid + "'\nPassword: ");
    return 0;
}

bool SystemController::wifi_join(const String& ssid, const String& pwd) {
    DBG_PRINTF(SystemController, "wifi_join(ssid=\"%s\", pwd=\"%s\")\n", ssid.c_str(), pwd.c_str());
    if (!wifi_module_active) {
        serial_port.println("WiFi Module disabled\n Use $wifi enable");
        return false;
    }
    serial_port.println("Connecting to '" + ssid + "'...");
    if (wifi.connect(ssid, pwd)) {
        wifi_status();
        nvs.write_bool("wifi_setup", true);
        nvs.write_str("wifi_name", ssid);
        nvs.write_str("wifi_pass", pwd);

        return true;
    }
    serial_port.println("Failed to connect to '" + ssid + "'");
    return false;
}

bool SystemController::wifi_disconnect() {
    DBG_PRINTLN(SystemController, "wifi_disconnect()");
    if (!wifi_module_active) {
        serial_port.println("WiFi Module disabled\n Use $wifi enable");
        return true;
    }
    if (!wifi.is_connected()) {
        serial_port.println("Not currently connected to WiFi");
        return true;
    }
    String current_ssid = wifi.get_ssid();
    wifi.disconnect();
    serial_port.println("Disconnected from '" + current_ssid + "'");
    return true;
}

bool SystemController::wifi_reset(bool print_info) {
    DBG_PRINTLN(SystemController, "wifi_reset()");
    nvs.write_bool("wifi_setup", false);
    nvs.write_str("wifi_name", "");
    nvs.write_str("wifi_pass", "");

    if (wifi.is_connected()) {
        wifi.disconnect();
    }
    if (print_info) {
        serial_port.println("WiFi credentials have been reset.\nUse '$wifi connect' to select a new network");
    }
    return true;
}

std::vector<String> SystemController::wifi_get_available_networks() {
    DBG_PRINTLN(SystemController, "wifi_get_available_networks()");
    if (!wifi_module_active) {
        serial_port.println("WiFi Module disabled\n Use $wifi enable");
        return {""};
    }
    serial_port.println("\nScanning available networks...");
    std::vector<String> networks = wifi.get_available_networks();
    serial_port.println("Available networks:");
    for (size_t i = 0; i < networks.size(); ++i) {
        serial_port.println(String(i) + ": " + networks[i]);
    }
    return networks;
}


/// RAM ///
void SystemController::ram_status() {
    DBG_PRINTLN(SystemController, "ram_status()");
    serial_port.print_spacer();
    uint32_t total       = ESP.getHeapSize();
    uint32_t free_bytes  = ESP.getFreeHeap();
    uint32_t min_free    = ESP.getMinFreeHeap();
    uint32_t max_alloc   = ESP.getMaxAllocHeap();
    uint32_t used        = total - free_bytes;
    float    pct_used    = used * 100.0f / total;
    float    pct_free    = free_bytes * 100.0f / total;
    serial_port.println("=== RAM Status ===");
    serial_port.print("Total heap:             ");
    serial_port.println(String(total));
    serial_port.print("Free heap:              ");
    serial_port.println(String(free_bytes));
    serial_port.print("Used heap:              ");
    serial_port.println(String(used));
    serial_port.print("Min ever free heap:     ");
    serial_port.println(String(min_free));
    serial_port.print("Max allocatable block:  ");
    serial_port.println(String(max_alloc));
    const uint8_t bar_width = 10;
    uint8_t used_bars = (uint8_t)((pct_used * bar_width / 100.0f) + 0.5f);
    serial_port.print("Usage: |");
    for (uint8_t i = 0; i < used_bars; ++i) {
        serial_port.print("x");
    }
    for (uint8_t i = used_bars; i < bar_width; ++i) {
        serial_port.print(".");
    }
    serial_port.print("| ");
    serial_port.println(String(pct_used, 1) + "%");
    uint8_t free_bars = bar_width - used_bars;
    serial_port.print("Free : |");
    for (uint8_t i = 0; i < free_bars; ++i) {
        serial_port.print("x");
    }
    for (uint8_t i = free_bars; i < bar_width; ++i) {
        serial_port.print(".");
    }
    serial_port.print("| ");
    serial_port.println(String(pct_free, 1) + "%");
    serial_port.print_spacer();
}

void SystemController::ram_free() {
    DBG_PRINTLN(SystemController, "ram_free()");
    serial_port.print("Free heap: ");
    serial_port.println(String(ESP.getFreeHeap()));
}

void SystemController::ram_watch(const String& args) {
    DBG_PRINTF(SystemController, "ram_watch(args=\"%s\")\n", args.c_str());
    uint32_t interval = (uint32_t)args.toInt();
    serial_port.println("Entering RAM watch. Press any key to exit.");

    ram_print_ticker.attach(int(interval / 1000), [this]() {
        serial_port.print("Free heap: ");
        serial_port.println(String(ESP.getFreeHeap()));
  });
}


// Web Interface Strip Functions
void SystemController::webinterface_enable(bool force_enable, bool force_restart) {
    system_module_enable(
        webinterface_module_active,
        "Web Interface Module", // Using consistent, full name
        "webint_mod_act",
        "This allows LED control via browser",
        force_enable,
        wifi_module_active,
        "Web Interface Module requires WiFi\nUse $wifi enable",
        force_restart
    );
}

void SystemController::webinterface_disable(bool force_disable, bool force_restart) {
    system_module_disable(
        webinterface_module_active,
        "Web Interface Module", // Using consistent, full name
        "webint_mod_act",
        force_disable,
        force_restart
    );
}

void SystemController::webinterface_reset() {
    if(!webinterface_module_active){
        serial_port.println("Web Interface Module disabled\nUse $webinterface enable");
        return;
    }

    webinterface.reset();
}

void SystemController::webinterface_status() {
    if(!webinterface_module_active){
        serial_port.println("Web Interface Module disabled\nUse $webinterface enable");
        return;
    }
    webinterface.status();
}

// Alexa Integration Functions
void SystemController::alexa_enable(bool force_enable, bool force_restart) {
    system_module_enable(
        alexa_module_active,
        "Alexa Module", // Using consistent, full name
        "alexa_mod_act",
        "This allows LED control via Amazon Alexa",
        force_enable,
        wifi_module_active,
        "Alexa Module requires WiFi\nUse $wifi enable",
        force_restart
    );
}

void SystemController::alexa_disable(bool force_disable, bool force_restart) {
    system_module_disable(
        alexa_module_active,
        "Alexa Module", // Using consistent, full name
        "alexa_mod_act",
        force_disable,
        force_restart
    );
}

void SystemController::alexa_reset() {
    if(!alexa_module_active){
        serial_port.println("Alexa Module disabled\nUse $alexa enable");
        return;
    }

    alexa.reset();
}

void SystemController::alexa_status() {
    if(!alexa_module_active){
        serial_port.println("Alexa Module disabled\nUse $alexa enable");
        return;
    }

    alexa.status();
}

// HomeKit Integration Functions
void SystemController::homekit_enable(bool force_enable, bool force_restart) {
    system_module_enable(
        homekit_module_active,
        "HomeKit Module", // Using consistent, full name
        "homekit_mod_act",
        "This allows LED control via Apple Home App",
        force_enable,
        wifi_module_active,
        "HomeKit Module requires WiFi\nUse $wifi enable",
        force_restart
    );
}

void SystemController::homekit_disable(bool force_disable, bool force_restart) {
    system_module_disable(
        homekit_module_active,
        "HomeKit Module",
        "homekit_mod_act",
        force_disable,
        force_restart
    );
}

void SystemController::homekit_reset() {
    if(!homekit_module_active){
        serial_port.println("HomeKit Module disabled\nUse $homekit enable");
        return;
    }
    homekit.reset();
}

void SystemController::homekit_status() {
    if(!homekit_module_active){
        serial_port.println("HomeKit Module disabled\nUse $homekit enable");
        return;
    }
    homekit.status();
}

void SystemController::system_module_enable(
    bool& active_flag,
    const char* module_name_full,
    const char* memory_key,
    const char* prompt_details,
    bool force_enable,
    const bool& dependency_flag,
    const char* dependency_error_msg,
    bool requires_restart,
    std::function<void()> on_enable_action,
    const char* already_enabled_msg
) {
    if (dependency_error_msg != nullptr && !dependency_flag) {
        serial_port.println(dependency_error_msg);
        return;
    }

    if (force_enable) {
        nvs.write_bool(memory_key, true);
        active_flag = true;
        if (on_enable_action) {
            on_enable_action();
        }
        serial_port.print(module_name_full);
        serial_port.println(" enabled");
        return;
    }

    if (active_flag) {
        // Use the custom "already enabled" message if provided
        if (already_enabled_msg != nullptr) {
            serial_port.println(already_enabled_msg);
        } else {
            serial_port.print(module_name_full);
            serial_port.println(" already enabled");
        }
        return;
    }

    char prompt_buffer[256];
    snprintf(prompt_buffer, sizeof(prompt_buffer), "Would you like to enable %s?\n%s", module_name_full, prompt_details);

    if (serial_port.prompt_user_yn(prompt_buffer)) {
        active_flag = true;
        nvs.write_bool(memory_key, true);

        // Execute the custom callback action if it exists
        if (on_enable_action) {
            on_enable_action();
        }

        if (requires_restart) {
            char message_buffer[128];
            snprintf(message_buffer, sizeof(message_buffer), "\n%s enabled\nPress enter to restart", module_name_full);
            serial_port.get_string(message_buffer);
            system_restart(1000);
        } else {
            serial_port.print("\n");
            serial_port.print(module_name_full);
            serial_port.println(" enabled");
        }
    }
}

void SystemController::system_module_disable(
    bool& active_flag,
    const char* module_name_full,
    const char* memory_key,
    bool force_disable,
    bool requires_restart,
    std::function<void()> on_disable_action
) {
    if (force_disable) {
        nvs.write_bool(memory_key, false);
        active_flag = false;
        serial_port.print(module_name_full);
        serial_port.println(" disabled");

        if (on_disable_action) {
            on_disable_action();
        }
        return;
    }

    if (!active_flag) {
        serial_port.print(module_name_full);
        serial_port.println(" already disabled");
        return;
    }

    char prompt_buffer[128];
    snprintf(prompt_buffer, sizeof(prompt_buffer), "Would you like to disable %s?", module_name_full);

    if (serial_port.prompt_user_yn(prompt_buffer)) {
        active_flag = false;
        nvs.write_bool(memory_key, false);

        // Execute the custom callback action if it exists
        if (on_disable_action) {
            on_disable_action();
        }

        if (requires_restart) {
            char message_buffer[128];
            snprintf(message_buffer, sizeof(message_buffer), "\n%s disabled\nPress enter to restart", module_name_full);
            serial_port.get_string(message_buffer);
            system_restart(1000);
        } else {
            serial_port.print("\n");
            serial_port.print(module_name_full);
            serial_port.println(" disabled");
        }
    }
}


// --- NEW: Command Execution Callback from ButtonController ---
void SystemController::execute_command(const String& command) {
    serial_port.println("<- BTN: " + command);
    command_parser.loop(command);
}

// --- NEW: Button command implementations ---
void SystemController::buttons_enable(bool force_enable, bool force_restart) {
    system_module_enable(
        buttons_module_active,
        "Button Module",
        "buttons_mod_act",
        "This allows controlling LEDs with physical buttons",
        force_enable,
        true, // No dependency
        nullptr,
        force_restart
    );
}

void SystemController::buttons_disable(bool force_disable, bool force_restart) {
    system_module_disable(
        buttons_module_active,
        "Button Module",
        "buttons_mod_act",
        force_disable,
        force_restart
    );
}


// The status command now gets the live status from the buttons object and prints it
void SystemController::buttons_status() {
    if (!buttons_module_active) {
        serial_port.println("Buttons Module is disabled. Use '$buttons enable'");
        return;
    }
    serial_port.println("\n--- Saved Button Configurations (NVS) ---");
    int btn_count = nvs.read_uint8("btn_count", 0);
    if (btn_count == 0) {
        serial_port.println("No buttons configured in storage.");
    } else {
        for (int i = 0; i < btn_count; i++) {
            String key = "btn_cfg_" + String(i);
            serial_port.println("  - " + nvs.read_str(key.c_str()));
        }
    }
    serial_port.println("-----------------------------------------");
    serial_port.println(buttons.get_live_status()); // Get live status and print
}

// The reset command handles all NVS logic before calling the simple reset on the buttons object
void SystemController::buttons_reset() {
    if (!buttons_module_active) {
        serial_port.println("Buttons Module is disabled. Use '$buttons enable'");
        return;
    }
    if (serial_port.prompt_user_yn("Are you sure you want to delete ALL button configurations?")) {
        int btn_count = nvs.read_uint8("btn_count", 0);
        for (int i = 0; i < btn_count; i++) {
            nvs.remove(("btn_cfg_" + String(i)).c_str());
        }
        nvs.write_uint8("btn_count", 0);
        nvs.commit();

        buttons.reset(); // Clear live buttons
        serial_port.println("All button configurations have been reset.");
        serial_port.get_string("Press enter to restart for changes to take full effect.");
        system_restart(100);
    }
}

// The add command handles all NVS logic and calls the simple add function on the buttons object
void SystemController::buttons_add(const String& args) {
    if (!buttons_module_active) {
        serial_port.println("Buttons Module is disabled. Use '$buttons enable'");
        return;
    }

    // Check for duplicates in NVS
    // here need to strip "" in case they are present
    int first_space = args.indexOf(' ');
    String pin_str = (first_space != -1) ? args.substring(0, first_space) : args;
    int btn_count = nvs.read_uint8("btn_count", 0);
    for (int i = 0; i < btn_count; i++) {
        String existing_config = nvs.read_str(("btn_cfg_" + String(i)).c_str());
        if (existing_config.startsWith(pin_str + " ")) {
            serial_port.println("Error: A button is already configured on pin " + pin_str + ".");
            return;
        }
    }

    // Attempt to add to live controller to validate config
    if (buttons.add_button_from_config(args)) {
        nvs.write_str(("btn_cfg_" + String(btn_count)).c_str(), args);
        nvs.write_uint8("btn_count", btn_count + 1);
        serial_port.println("Successfully added button action: " + args);
    } else {
        serial_port.println("Error: Invalid button configuration string.");
    }
}

// The remove command handles all NVS logic before calling the simple remove function on the buttons object
void SystemController::buttons_remove(const String& pin_str) {
    if (!buttons_module_active) {
        serial_port.println("Buttons Module is disabled. Use '$buttons enable'");
        return;
    }
    if (pin_str.isEmpty()) {
        serial_port.println("Error: Invalid pin number provided.");
        return;
    }

    uint8_t pin_to_remove = pin_str.toInt();
    int btn_count = nvs.read_uint8("btn_count", 0);
    int found_index = -1;

    // Find in NVS
    for (int i = 0; i < btn_count; i++) {
        String config = nvs.read_str(("btn_cfg_" + String(i)).c_str());
        if (config.startsWith(pin_str + " ")) {
            found_index = i;
            break;
        }
    }

    if (found_index == -1) {
        serial_port.println("Error: No button found on pin " + pin_str);
        return;
    }

    // Shift configs in NVS
    for (int i = found_index; i < btn_count - 1; i++) {
        String next_config = nvs.read_str(("btn_cfg_" + String(i + 1)).c_str());
        nvs.write_str(("btn_cfg_" + String(i)).c_str(), next_config);
    }

    nvs.remove(("btn_cfg_" + String(btn_count - 1)).c_str());
    nvs.write_uint8("btn_count", btn_count - 1);
    nvs.commit();

    buttons.remove_button(pin_to_remove); // Remove from live
    serial_port.println("Successfully removed button on pin " + pin_str);
}
