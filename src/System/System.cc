// System.cpp
#include "System.h"
#include <vector>

System::System()
  : serialPort(115200)
  , wifi("ESP32-C3-Device")
  , memory(512)
  , wifi_connected(false)
{}

void System::init_system_setup() {
    serialPort.println("Welcome to the XeWe Led OS!");
    serialPort.println("This is a complete OS solution to control the addressable LED lights.");
    serialPort.println("Communication supported: serial port");
    serialPort.println("Communication to be supported: Webserver, Alexa, Homekit, Yandex-Alisa");

    serialPort.println("Let's connect to WiFi:");
    connect_wifi();
}

// Command parsing

void System::update() {
    command_parser.parse();

}


// WiFi
bool System::connect_wifi() {
    String ssid, pwd;

    // 1) Try stored credentials first
    if (!read_memory_wifi_credentials(ssid, pwd)) {
        // No valid stored creds or connect failed → prompt user
        prompt_user_for_wifi_credentials(ssid, pwd);
    }

    while (!wifi_connected) {
        serialPort.print("Connecting to ");
        serialPort.println(ssid);

        // Attempt connect
        if (wifi.connect(ssid, pwd)) {
            serialPort.print("Connected to ");
            serialPort.println(ssid);
            serialPort.print("Local ip: ");
            serialPort.println(wifi.get_local_ip());

            // Store for next boot
            memory.write_bit("wifi_flags", 0, 1);
            memory.write_str("wifi_name", ssid);
            memory.write_str("wifi_pass", pwd);
            wifi_connected = true;
            return true;
        }
        else {
            serialPort.print("Failed to connect to ");
            serialPort.println(ssid);
            serialPort.println("Let's try again.");
            prompt_user_for_wifi_credentials(ssid, pwd);
        }
    }
    return false;  // never reached
}

bool System::read_memory_wifi_credentials(String& ssid, String& pwd) {
    if (!memory.read_bit("wifi_flags", 0)) {
        return false;
    }
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
    // Saved creds were bad
    serialPort.println("Stored credentials failed; discarding.");
    return false;
}

bool System::prompt_user_for_wifi_credentials(String& ssid, String& pwd) {
    // Clear old flag so we know to save new ones
    memory.write_bit("wifi_flags", 0, 0);
    serialPort.println("Please enter new WiFi credentials:");

    // 1) Scan & list
    auto networks = wifi.get_available_networks();
    serialPort.println("Available networks:");
    serialPort.println("0: Enter custom SSID");
    for (size_t i = 0; i < networks.size(); ++i) {
        serialPort.print(String(i + 1) + ": ");
        serialPort.println(networks[i]);
    }

    // 2) Ask selection
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

    // 3) Ask password
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
