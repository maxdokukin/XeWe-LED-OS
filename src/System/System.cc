// System.cc
#include "System.h"

// ——— System ctor ———
System::System()
  : serial_port(115200)
  , wifi("ESP32-C3-Device")
  , memory(512)
{}

// ——— init_system_setup ———
void System::init_system_setup() {
    serial_port.println("Welcome to the XeWe Led OS!");
    serial_port.println("This is a complete OS solution to control the addressable LED lights.");
    serial_port.println("Communication supported: serial port");
    serial_port.println("Communication to be supported: Webserver, Alexa, Homekit, Yandex-Alisa");

    define_commands();

    serial_port.println("Let's connect to WiFi:");
    connect_wifi();
}

// ——— update() ———
void System::update() {
    if (serial_port.has_line()) {
        String line = serial_port.read_line();
        serial_port.println(line);
        command_parser.parse_and_execute(line);
    }
}

// ——— define_commands ———
void System::define_commands() {
    wifi_commands[0].name     = "help";
    wifi_commands[0].function = [this](const String&) { print_wifi_help(); };

    wifi_commands[1].name     = "connect";
    wifi_commands[1].function = [this](const String&) { connect_wifi(); };

    wifi_commands[2].name     = "disconnect";
    wifi_commands[2].function = [this](const String&) { disconnect_wifi(); };

    wifi_commands[3].name     = "reset_credentials";
    wifi_commands[3].function = [this](const String&) { reset_wifi_credentials(); };

    wifi_commands[4].name     = "status";
    wifi_commands[4].function = [this](const String&) { print_wifi_credentials(); };

    wifi_commands[5].name     = "scan";
    wifi_commands[5].function = [this](const String&) {
        serial_port.println("Scanning available networks...");
        auto networks = wifi.get_available_networks();
        serial_port.println("Available networks:");
        for (size_t i = 0; i < networks.size(); ++i) {
            serial_port.print(String(i + 1) + ": ");
            serial_port.println(networks[i]);
        }
    };

    wifi_group.name          = "wifi";
    wifi_group.commands      = wifi_commands;
    wifi_group.command_count  = WIFI_CMD_COUNT;
    command_parser.set_groups(&wifi_group, 1);
}

// ——— connect_wifi ———
bool System::connect_wifi() {
    String ssid, pwd;

    if (wifi.is_connected()) {
        serial_port.println("Already connected.");
        print_wifi_credentials();
        serial_port.println("Use 'wifi reset_credentials' to change network.");
        return true;
    }

    // either load or prompt
    if (!read_memory_wifi_credentials(ssid, pwd)) {
        if (!prompt_user_for_wifi_credentials(ssid, pwd)) {
            serial_port.println("WiFi setup aborted by user.");
            return false;
        }
    }

    // attempt loop
    while (!wifi.is_connected()) {
        serial_port.print("Connecting to '");
        serial_port.print(ssid);
        serial_port.println("'...");

        if (wifi.connect(ssid, pwd)) {
            print_wifi_credentials();
            memory.write_bit("wifi_flags", 0, 1);
            memory.write_str("wifi_name", ssid);
            memory.write_str("wifi_pass", pwd);
            return true;
        }

        serial_port.print("Failed to connect to '");
        serial_port.print(ssid);
        serial_port.println("'.");
        serial_port.println("Let's try again.");

        if (!prompt_user_for_wifi_credentials(ssid, pwd)) {
            serial_port.println("WiFi setup aborted by user.");
            return false;
        }
    }

    return false;
}

// ——— print_wifi_credentials ———
void System::print_wifi_credentials() {
    if(!wifi.is_connected()){
        serial_port.println("WiFi not connected");
        return;
    }

    serial_port.print("Connected to ");
    serial_port.println(wifi.get_ssid());
    serial_port.print("Local ip: ");
    serial_port.println(wifi.get_local_ip());
}

// ——— read_memory_wifi_credentials ———
bool System::read_memory_wifi_credentials(String& ssid, String& pwd) {
    if (!memory.read_bit("wifi_flags", 0)) {
        return false;
    }

    serial_port.println("Found saved WiFi credentials");
    ssid = memory.read_str("wifi_name");
    pwd  = memory.read_str("wifi_pass");

    serial_port.print("Connecting to '");
    serial_port.print(ssid);
    serial_port.println("'...");

    if (wifi.connect(ssid, pwd)) {
        print_wifi_credentials();
        return true;
    }

    serial_port.println("Stored credentials failed; discarding.");
    return false;
}

// ——— prompt_user_for_wifi_credentials ———
bool System::prompt_user_for_wifi_credentials(String& ssid, String& pwd) {
    memory.write_bit("wifi_flags", 0, 0);

    serial_port.println("Scanning available networks...");
    auto networks = wifi.get_available_networks();

    serial_port.println("Available networks:");
    serial_port.println("0: Enter custom SSID");
    for (size_t i = 0; i < networks.size(); ++i) {
        serial_port.print(String(i + 1) + ": ");
        serial_port.println(networks[i]);
    }

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
        return false;
    }

    // get password once
    serial_port.print("Enter password for '");
    serial_port.print(ssid);
    serial_port.println("':");
    pwd = serial_port.get_string();

    return true;
}

// ——— disconnect_wifi ———
bool System::disconnect_wifi() {
    if (!wifi.is_connected()) {
        serial_port.println("Not currently connected to WiFi.");
        return false;
    }

    wifi.disconnect();
    serial_port.println("Disconnected from WiFi.");
    return true;
}

// ——— reset_wifi_credentials ———
bool System::reset_wifi_credentials() {
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
    return true;
}

// ——— print_wifi_help ———
void System::print_wifi_help() {
    serial_port.println("WiFi commands:");
    serial_port.println("  $help              - Show this help message");
    serial_port.println("  $connect           - Connect or reconnect to WiFi");
    serial_port.println("  $disconnect        - Disconnect from WiFi");
    serial_port.println("  $reset_credentials - Clear saved WiFi credentials");
    serial_port.println("  $status            - Show connection status & IP");
    serial_port.println("  $scan              - List available WiFi networks");
}
