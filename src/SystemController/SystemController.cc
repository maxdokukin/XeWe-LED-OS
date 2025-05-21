// SystemController.cc
#include "SystemController.h"

// ——— System ctor ———
SystemController::SystemController(Adafruit_NeoPixel* strip)
  : serial_port(115200)
  , wifi("ESP32-C3-Device")
  , memory(512)
  , led_controller(strip, memory.read_byte("led_strip_r"), memory.read_byte("led_strip_g"), memory.read_byte("led_strip_b"), memory.read_byte("led_strip_brightness"), memory.read_byte("led_strip_state"), memory.read_byte("led_strip_mode"))
{
}

// ——— init_system_setup ———
void SystemController::init_system_setup() {
    serial_port.println("\n\n\n");
    serial_port.print("+------------------------------------------------+\n"
                      "|          Welcome to the XeWe Led OS            |\n"
                      "+------------------------------------------------+\n"
                      "|   This is a complete OS solution to control    |\n"
                      "|            addressable LED lights.             |\n"
                      "+------------------------------------------------+\n"
                      "|      Communication supported: serial port      |\n"
                      "|         Communication to be supported:         |\n"
                      "|     Webserver, Alexa, Homekit, Yandex-Alisa    |\n"
                      "+------------------------------------------------+\n");

    define_commands();

    wifi_connect();

}

// ——— update() ———
void SystemController::update() {
    if (serial_port.has_line()) {
        String line = serial_port.read_line();
        serial_port.println(line);
        command_parser.parse_and_execute(line);
    }

    led_controller.frame();
}

// ——— define_commands ———
void SystemController::define_commands() {
    // populate Wi-Fi commands
    wifi_commands[0] = { "help",              "Show this help message",                0, [this](auto&){ wifi_print_help(); } };
    wifi_commands[1] = { "connect",           "Connect or reconnect to WiFi",          0, [this](auto&){ wifi_connect(); } };
    wifi_commands[2] = { "disconnect",        "Disconnect from WiFi",                  0, [this](auto&){ disconnect_wifi(); } };
    wifi_commands[3] = { "reset_credentials", "Clear saved WiFi credentials",          0, [this](auto&){ reset_wifi_credentials(); } };
    wifi_commands[4] = { "status",            "Show connection status, SSID, IP, MAC", 0, [this](auto&){ wifi_print_credentials(); } };
    wifi_commands[5] = { "scan",              "List available WiFi networks",          0, [this](auto&){ wifi_get_available_networks(); } };

    // populate LED-strip commands
    led_strip_commands[0]  = { "help",           "Show this help message",      0, [this](auto&){ led_strip_print_help(); } };
    led_strip_commands[1]  = { "set_mode",       "Set LED strip mode",          1, [this](auto& a){ led_strip_set_mode(a); } };
    led_strip_commands[2]  = { "set_rgb",        "Set RGB color",               3, [this](auto& a){ led_strip_set_rgb(a); } };
    led_strip_commands[3]  = { "set_r",          "Set red channel",             1, [this](auto& a){ led_strip_set_r(a); } };
    led_strip_commands[4]  = { "set_g",          "Set green channel",           1, [this](auto& a){ led_strip_set_g(a); } };
    led_strip_commands[5]  = { "set_b",          "Set blue channel",            1, [this](auto& a){ led_strip_set_b(a); } };
    led_strip_commands[6]  = { "set_hsv",        "Set HSV color",               3, [this](auto& a){ led_strip_set_hsv(a); } };
    led_strip_commands[7]  = { "set_hue",        "Set hue channel",             1, [this](auto& a){ led_strip_set_hue(a); } };
    led_strip_commands[8]  = { "set_sat",        "Set saturation channel",      1, [this](auto& a){ led_strip_set_sat(a); } };
    led_strip_commands[9]  = { "set_val",        "Set value channel",           1, [this](auto& a){ led_strip_set_val(a); } };
    led_strip_commands[10]  = { "set_brightness", "Set global brightness",       1, [this](auto& a){ led_strip_set_brightness(a); } };
    led_strip_commands[11] = { "set_state",      "Set on/off state",            1, [this](auto& a){ led_strip_set_state(a); } };
    led_strip_commands[12] = { "turn_on",        "Turn strip on",               0, [this](auto&){ led_strip_turn_on(); } };
    led_strip_commands[13] = { "turn_off",       "Turn strip off",              0, [this](auto&){ led_strip_turn_off(); } };

    // populate groups
    command_groups[0] = { "wifi", wifi_commands,      WIFI_CMD_COUNT };
    command_groups[1] = { "led",  led_strip_commands, LED_STRIP_CMD_COUNT };

    // register
    command_parser.set_groups(command_groups, CMD_GROUP_COUNT);
}

// ——— LED handlers ———
void SystemController::led_strip_print_help() {
    serial_port.print_spacer();
    serial_port.println("Led commands:");
    for (size_t i = 0; i < LED_STRIP_CMD_COUNT; ++i) {
        const auto &cmd = led_strip_commands[i];
        serial_port.print("  $");
        serial_port.print(cmd.name);
        serial_port.print(" - ");
        serial_port.print(cmd.description);
        serial_port.print(", argument count: ");
        serial_port.println(String(cmd.arg_count));
    }
    serial_port.print_spacer();}

void SystemController::led_strip_set_mode(const String& args) {
    uint8_t mode = static_cast<uint8_t>(args.toInt());
    led_controller.set_mode(mode);
    memory.write_byte("led_strip_mode", mode);
}

void SystemController::led_strip_set_rgb(const String& args) {
    int i1 = args.indexOf(' '), i2 = args.indexOf(' ', i1 + 1);
    uint8_t r = args.substring(0, i1).toInt();
    uint8_t g = args.substring(i1 + 1, i2).toInt();
    uint8_t b = args.substring(i2 + 1).toInt();
    led_controller.set_rgb(r, g, b);
    memory.write_byte("led_strip_r", led_controller.get_r());
    memory.write_byte("led_strip_g", led_controller.get_g());
    memory.write_byte("led_strip_b", led_controller.get_b());
}

void SystemController::led_strip_set_r(const String& args) {
    led_controller.set_r(static_cast<uint8_t>(args.toInt()));
    memory.write_byte("led_strip_r", led_controller.get_r());
}

void SystemController::led_strip_set_g(const String& args) {
    led_controller.set_g(static_cast<uint8_t>(args.toInt()));
    memory.write_byte("led_strip_g", led_controller.get_g());
}

void SystemController::led_strip_set_b(const String& args) {
    led_controller.set_b(static_cast<uint8_t>(args.toInt()));
    memory.write_byte("led_strip_b", led_controller.get_b());
}

void SystemController::led_strip_set_hsv(const String& args) {
    int i1 = args.indexOf(' '), i2 = args.indexOf(' ', i1 + 1);
    uint8_t h = args.substring(0, i1).toInt();
    uint8_t s = args.substring(i1 + 1, i2).toInt();
    uint8_t v = args.substring(i2 + 1).toInt();
    led_controller.set_hsv(h, s, v);
    memory.write_byte("led_strip_r", led_controller.get_r());
    memory.write_byte("led_strip_g", led_controller.get_g());
    memory.write_byte("led_strip_b", led_controller.get_b());
}

void SystemController::led_strip_set_hue(const String& args) {
    led_controller.set_hue(static_cast<uint8_t>(args.toInt()));
    memory.write_byte("led_strip_r", led_controller.get_r());
    memory.write_byte("led_strip_g", led_controller.get_g());
    memory.write_byte("led_strip_b", led_controller.get_b());
}

void SystemController::led_strip_set_sat(const String& args) {
    led_controller.set_sat(static_cast<uint8_t>(args.toInt()));
    memory.write_byte("led_strip_r", led_controller.get_r());
    memory.write_byte("led_strip_g", led_controller.get_g());
    memory.write_byte("led_strip_b", led_controller.get_b());
}

void SystemController::led_strip_set_val(const String& args) {
    led_controller.set_val(static_cast<uint8_t>(args.toInt()));
    memory.write_byte("led_strip_r", led_controller.get_r());
    memory.write_byte("led_strip_g", led_controller.get_g());
    memory.write_byte("led_strip_b", led_controller.get_b());
}

void SystemController::led_strip_set_brightness(const String& args) {
    led_controller.set_brightness(static_cast<uint8_t>(args.toInt()));
    memory.write_byte("led_strip_brightness", static_cast<uint8_t>(args.toInt()));
}

void SystemController::led_strip_set_state(const String& args) {
    led_controller.set_state(static_cast<byte>(args.toInt()));
    memory.write_byte("led_strip_state", static_cast<uint8_t>(args.toInt()));
}

void SystemController::led_strip_turn_on() {
    led_controller.turn_on();
    memory.write_byte("led_strip_state", 1);
}

void SystemController::led_strip_turn_off() {
    led_controller.turn_off();
    memory.write_byte("led_strip_state", 0);
}



//////WIFI/////
// ——— wifi_connect ———
bool SystemController::wifi_connect() {
    serial_port.print_spacer();

    String ssid, pwd;
    if (wifi.is_connected()) {
        serial_port.println("Already connected.");
        wifi_print_credentials();
        serial_port.println("Use 'wifi reset_credentials' to change network.");
        serial_port.print_spacer();
        return true;
    }

    if (!read_memory_wifi_credentials(ssid, pwd)) {
        if (!prompt_user_for_wifi_credentials(ssid, pwd)) {
            serial_port.println("WiFi setup aborted by user.");
            serial_port.print_spacer();
            return false;
        }
    }

    while (!wifi.is_connected()) {
        serial_port.print("Connecting to '");
        serial_port.print(ssid);
        serial_port.println("'...");

        if (wifi.connect(ssid, pwd)) {
            wifi_print_credentials();
            memory.write_bit("wifi_flags", 0, 1);
            memory.write_str("wifi_name", ssid);
            memory.write_str("wifi_pass", pwd);
            serial_port.print_spacer();
            return true;
        }

        serial_port.print("Failed to connect to '");
        serial_port.print(ssid);
        serial_port.println("'.");
        serial_port.println("Let's try again.");

        if (!prompt_user_for_wifi_credentials(ssid, pwd)) {
            serial_port.println("WiFi setup aborted by user.");
            serial_port.print_spacer();
            return false;
        }
    }

    serial_port.print_spacer();
    return false;
}

// ——— wifi_print_credentials ———
void SystemController::wifi_print_credentials() {
    serial_port.print_spacer();

    if (!wifi.is_connected()) {
        serial_port.println("WiFi not connected");
        serial_port.print_spacer();
        return;
    }

    serial_port.print("Connected to ");
    serial_port.println(wifi.get_ssid());
    serial_port.print("Local ip: ");
    serial_port.println(wifi.get_local_ip());
    serial_port.print("MAC: ");
    serial_port.println(wifi.get_mac_address());

    serial_port.print_spacer();
}

// ——— read_memory_wifi_credentials ———
bool SystemController::read_memory_wifi_credentials(String& ssid, String& pwd) {
    if (!memory.read_bit("wifi_flags", 0)) {
        return false;
    }

    serial_port.print_spacer();
    serial_port.println("Found saved WiFi credentials");
    ssid = memory.read_str("wifi_name");
    pwd  = memory.read_str("wifi_pass");

    serial_port.print("Connecting to '");
    serial_port.print(ssid);
    serial_port.println("'...");

    if (wifi.connect(ssid, pwd)) {
        wifi_print_credentials();
        serial_port.print_spacer();
        return true;
    }

    serial_port.println("Stored credentials failed; discarding.");
    serial_port.print_spacer();
    return false;
}

// ——— prompt_user_for_wifi_credentials ———
bool SystemController::prompt_user_for_wifi_credentials(String& ssid, String& pwd) {
    serial_port.print_spacer();
    memory.write_bit("wifi_flags", 0, 0);

    std::vector<String> networks = wifi_get_available_networks();
    serial_port.println("Select network by number:");
    int choice = serial_port.get_int();

    if (choice == 0) {
        bool confirmed = false;
        while (!confirmed) {
            serial_port.println("Enter SSID:");
            ssid = serial_port.get_string();
            serial_port.print("Confirm SSID: ");
            serial_port.println(ssid);
            confirmed = serial_port.get_confirmation();
        }
    }
    else if (choice > 0 && choice <= (int)networks.size()) {
        ssid = networks[choice - 1];
    }
    else {
        serial_port.println("Invalid choice. Aborting entry.");
        serial_port.print_spacer();
        return false;
    }

    serial_port.print("Enter password for '");
    serial_port.print(ssid);
    serial_port.println("':");
    pwd = serial_port.get_string();

    serial_port.print_spacer();
    return true;
}

// ——— disconnect_wifi ———
bool SystemController::disconnect_wifi() {
    serial_port.print_spacer();

    if (!wifi.is_connected()) {
        serial_port.println("Not currently connected to WiFi.");
        serial_port.print_spacer();
        return false;
    }

    wifi.disconnect();
    serial_port.println("Disconnected from WiFi.");
    serial_port.print_spacer();
    return true;
}

// ——— reset_wifi_credentials ———
bool SystemController::reset_wifi_credentials() {
    serial_port.print_spacer();

    memory.write_bit("wifi_flags", 0, 0);
    memory.write_str("wifi_name", "");
    memory.write_str("wifi_pass", "");

    if (wifi.is_connected()) {
        wifi.disconnect();
    }

    serial_port.println(
        "WiFi credentials have been reset. "
        "Use 'wifi connect' to select a new network."
    );
    serial_port.print_spacer();
    return true;
}

// ——— wifi_get_available_networks ———
std::vector<String> SystemController::wifi_get_available_networks() {
    serial_port.print_spacer();

    serial_port.println("Scanning available networks...");
    std::vector<String> networks = wifi.get_available_networks();

    serial_port.println("Available networks:");
    serial_port.println("0: Enter custom SSID");
    for (size_t i = 0; i < networks.size(); ++i) {
        serial_port.print(String(i + 1) + ": ");
        serial_port.println(networks[i]);
    }

    serial_port.print_spacer();
    return networks;
}

// ——— wifi_print_help ———
void SystemController::wifi_print_help() {
    serial_port.print_spacer();
    serial_port.println("WiFi commands:");
    for (size_t i = 0; i < WIFI_CMD_COUNT; ++i) {
        const auto &cmd = wifi_commands[i];
        serial_port.print("  $");
        serial_port.print(cmd.name);
        serial_port.print(" - ");
        serial_port.print(cmd.description);
        serial_port.print(", argument count: ");
        serial_port.println(String(cmd.arg_count));
    }
    serial_port.print_spacer();
}

