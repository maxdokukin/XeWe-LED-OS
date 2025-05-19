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

    // Explicitly assign each command’s fields
    wifiCommands[0].name = "connect";
    wifiCommands[0].function = [this](const String&) {
        connect_wifi();
    };

    wifiCommands[1].name = "status";
    wifiCommands[1].function = [this](const String&) {
        if (wifi_connected) {
            serialPort.print("Connected. IP: ");
            serialPort.println(wifi.get_local_ip());
        } else {
            serialPort.println("WiFi not connected");
        }
    };

    wifiCommands[2].name = "scan";
    wifiCommands[2].function = [this](const String&) {
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

//    serialPort.println("Commands: wifi connect | wifi status | wifi scan");
//    serialPort.println("Or later run 'wifi connect' to (re)enter credentials.");
    serialPort.println("Let's connect to WiFi:");
    // Optionally auto-connect on startup
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

// ——— Wi-Fi flows (unchanged) ———

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
