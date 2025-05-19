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

bool System::connect_wifi() {
    while (!wifi_connected) {
        // 1) Scan & list
        std::vector<String> networks = wifi.get_available_networks();
        serialPort.println("Available networks:");
        for (size_t i = 0; i < networks.size(); ++i) {
            serialPort.print(String(i + 1) + ": ");
            serialPort.println(networks[i]);
        }
        serialPort.println("0: Enter custom SSID");

        // 2) Ask selection
        serialPort.print("Select network by number: ");
        int choice = serialPort.get_int();

        String ssid;
        if (choice == 0) {
            bool confirmed = false;
            while (!confirmed) {
                serialPort.print("Enter network name: ");
                ssid = serialPort.get_string();
                serialPort.print("Confirm network name: ");
                serialPort.println(ssid);
                confirmed = serialPort.get_confirmation();
            }
        }
        else if (choice > 0 && choice <= (int)networks.size()) {
            ssid = networks[choice - 1];
        }
        else {
            serialPort.println("Invalid selection; try again.");
            continue;
        }

        // 3) Ask password
        String pwd;
        bool pwd_ok = false;
        while (!pwd_ok) {
            serialPort.print("Enter network password: ");
            pwd = serialPort.get_string();
            serialPort.print("Confirm network password: ");
            serialPort.println(pwd);
            pwd_ok = serialPort.get_confirmation();
        }

        // 4) Attempt connect
        if (wifi.connect(ssid, pwd)) {
            serialPort.print("Connected to ");
            serialPort.println(ssid);
            memory.write("wifi_name", ssid);
            memory.write("wifi_pass", pwd);
            wifi_connected = true;
            return true;
        }
        else {
            serialPort.print("Failed to connect to ");
            serialPort.println(ssid);
            serialPort.println("Let's try again.");
        }
    }
    return false;  // never reached, but placates the compiler
}
