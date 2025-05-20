// SystemController.cc
#include "SystemController.h"

// ——— System ctor ———
SystemController::SystemController()
  : serial_port(115200)
  , wifi("ESP32-C3-Device")
  , memory(512)
{}

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

    connect_wifi();
}

// ——— update() ———
void SystemController::update() {
    if (serial_port.has_line()) {
        String line = serial_port.read_line();
        serial_port.println(line);
        command_parser.parse_and_execute(line);
    }
}

// ——— define_commands ———
void SystemController::define_commands() {

    static const CommandParser::Command wifi_commands[WIFI_CMD_COUNT] = {
        { "help",              "Show this help message",                0, [this](const String&){ print_wifi_help(); } },
        { "connect",           "Connect or reconnect to WiFi",          0, [this](const String&){ connect_wifi(); } },
        { "disconnect",        "Disconnect from WiFi",                  0, [this](const String&){ disconnect_wifi(); } },
        { "reset_credentials", "Clear saved WiFi credentials",          0, [this](const String&){ reset_wifi_credentials(); } },
        { "status",            "Show connection status, SSID, IP, MAC", 0, [this](const String&){ print_wifi_credentials(); } },
        { "scan",              "List available WiFi networks",          0, [this](const String&){ get_available_wifi_networks(); } },
    };
    static const CommandParser::CommandGroup wifi_group = { "wifi", wifi_commands, WIFI_CMD_COUNT };

    // register with the parser
    command_parser.set_groups(&wifi_group, 1);
}


//////WIFI////
// ——— connect_wifi ———
bool SystemController::connect_wifi() {
    serial_port.print_spacer();

    String ssid, pwd;
    if (wifi.is_connected()) {
        serial_port.println("Already connected.");
        print_wifi_credentials();
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
            print_wifi_credentials();
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

// ——— print_wifi_credentials ———
void SystemController::print_wifi_credentials() {
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
        print_wifi_credentials();
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

    std::vector<String> networks = get_available_wifi_networks();
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

// ——— get_available_wifi_networks ———
std::vector<String> SystemController::get_available_wifi_networks() {
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

// ——— print_wifi_help ———
void SystemController::print_wifi_help() {
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

