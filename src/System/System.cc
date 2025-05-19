#include "System.h"

// ——— System ctor ———
System::System()
  : serialPort(115200)
  , wifi("ESP32-C3-Device")
  , memory(512)
  , wifi_connected(false)
  , command_parser()
{}

// ——— init_system_setup ———
void System::init_system_setup() {
    serialPort.println("Welcome to the XeWe Led OS!");
    serialPort.println("This is a complete OS solution to control the addressable LED lights.");
    serialPort.println("Communication supported: serial port");
    serialPort.println("Communication to be supported: Webserver, Alexa, Homekit, Yandex-Alisa");

    define_commands();

    serialPort.println("Let's connect to WiFi:");
    connect_wifi();
}

// ——— update() ———
void System::update() {
    if (serialPort.has_line()) {
        String line = serialPort.read_line();
        serialPort.println(line);
        command_parser.parse(line);
    }
}

// ——— define system commands ———
void System::define_commands() {
    // connect
    wifiCommands[0].name = "connect";
    wifiCommands[0].function = [this](const String&) {
        connect_wifi();
    };

    // disconnect
    wifiCommands[1].name = "disconnect";
    wifiCommands[1].function = [this](const String&) {
        disconnect_wifi();
    };

    // reset credentials
    wifiCommands[2].name = "reset_credentials";
    wifiCommands[2].function = [this](const String&) {
        reset_wifi_credentials();
    };

    // status
    wifiCommands[3].name = "status";
    wifiCommands[3].function = [this](const String&) {
        if (wifi_connected) {
            serialPort.print("Connected to ");
            serialPort.println(memory.read_str("wifi_name"));
            serialPort.print("Local ip: ");
            serialPort.println(wifi.get_local_ip());
        } else {
            serialPort.println("WiFi not connected");
        }
    };

    // scan
    wifiCommands[4].name = "scan";
    wifiCommands[4].function = [this](const String&) {
        auto networks = wifi.get_available_networks();
        serialPort.println("Available networks:");
        serialPort.println("0: Enter custom SSID");
        for (size_t i = 0; i < networks.size(); ++i) {
            serialPort.print(String(i + 1) + ": ");
            serialPort.println(networks[i]);
        }
    };

    // Build and register the single wifi group
    wifiGroup.name         = "wifi";
    wifiGroup.commands     = wifiCommands;
    wifiGroup.commandCount = WIFI_CMD_COUNT;

    command_parser.setGroups(&wifiGroup, 1);
}


// wifi functions
bool System::disconnect_wifi() {
    if (!wifi_connected) {
        serialPort.println("Not currently connected to WiFi.");
        return false;
    }

    wifi.disconnect();                        // tell the driver to drop the AP
    wifi_connected = false;
    serialPort.println("Disconnected from WiFi.");
    return true;
}

bool System::reset_wifi_credentials() {
    // Clear stored credentials
    memory.write_bit("wifi_flags", 0, 0);
    memory.write_str("wifi_name", "");
    memory.write_str("wifi_pass", "");

    // If we're currently connected, drop out
    if (wifi_connected) {
        wifi.disconnect();
        wifi_connected = false;
    }

    serialPort.println("WiFi credentials have been reset. You will be prompted to re-enter them on next connect.");
    return true;
}

bool System::connect_wifi() {
    String ssid, pwd;
    if (!read_memory_wifi_credentials(ssid, pwd)) {
        prompt_user_for_wifi_credentials(ssid, pwd);
    }
    while (!wifi_connected) {
        serialPort.print("Connecting to ");
        serialPort.println(ssid);
        if (wifi.connect(ssid, pwd)) {
            serialPort.print("Connected to ");
            serialPort.println(ssid);
            serialPort.print("Local ip: ");
            serialPort.println(wifi.get_local_ip());
            memory.write_bit("wifi_flags", 0, 1);
            memory.write_str("wifi_name", ssid);
            memory.write_str("wifi_pass", pwd);
            wifi_connected = true;
            return true;
        } else {
            serialPort.print("Failed to connect to ");
            serialPort.println(ssid);
            serialPort.println("Let's try again.");
            prompt_user_for_wifi_credentials(ssid, pwd);
        }
    }
    return false;
}

bool System::read_memory_wifi_credentials(String& ssid, String& pwd) {
    if (!memory.read_bit("wifi_flags", 0)) return false;

    serialPort.println("Found saved WiFi credentials");
    ssid = memory.read_str("wifi_name");
    pwd  = memory.read_str("wifi_pass");

    serialPort.print("Connecting to ");
    serialPort.println(ssid);
    if (wifi.connect(ssid, pwd)) {
        serialPort.print("Connected to ");
        serialPort.println(ssid);
        serialPort.print("Local ip: ");
        serialPort.println(wifi.get_local_ip());
        wifi_connected = true;
        return true;
    }
    serialPort.println("Stored credentials failed; discarding.");
    return false;
}

bool System::prompt_user_for_wifi_credentials(String& ssid, String& pwd) {
    memory.write_bit("wifi_flags", 0, 0);
    serialPort.println("Please enter new WiFi credentials:");

    auto networks = wifi.get_available_networks();
    serialPort.println("Available networks:");
    serialPort.println("0: Enter custom SSID");
    for (size_t i = 0; i < networks.size(); ++i) {
        serialPort.print(String(i + 1) + ": ");
        serialPort.println(networks[i]);
    }

    serialPort.println("Select network number:");
    int choice = serialPort.get_int();
    if (choice == 0) {
        bool ok = false;
        while (!ok) {
            serialPort.println("Enter SSID:");
            ssid = serialPort.get_string();
            serialPort.print("Confirm SSID: ");
            serialPort.println(ssid);
            ok = serialPort.get_confirmation();
        }
    }
    else if (choice > 0 && choice <= (int)networks.size()) {
        ssid = networks[choice - 1];
    }
    else {
        serialPort.println("Invalid choice. Aborting entry.");
        return false;
    }

    {
        bool ok = false;
        while (!ok) {
            serialPort.println("Enter password:");
            pwd = serialPort.get_string();
            serialPort.print("Confirm password: ");
            serialPort.println(pwd);
            ok = serialPort.get_confirmation();
        }
    }

    return true;
}
