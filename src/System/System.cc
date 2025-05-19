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

bool read_memory_wifi_credentials(String* ssid, String *pwd){
    if(memory.read_bit("wifi_flags", 0)){
        serialPort.println("Found WiFi credentials in the memory");

        String ssid = memory.read_str("wifi_name");
        String pwd = memory.read_str("wifi_pass");

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
    }
}

bool enter_wifi_credentials(String* ssid, String *pwd){
    memory.write_bit("wifi_flags", 0, 1);
    serialPort.println("Adding new WiFi credentials");

    // 1) Scan & list
    std::vector<String> networks = wifi.get_available_networks();
    serialPort.println("Available networks:");
    serialPort.println("0: Enter custom SSID");
    for (size_t i = 0; i < networks.size(); ++i) {
        serialPort.print(String(i + 1) + ": ");
        serialPort.println(networks[i]);
    }

    // 2) Ask selection
    serialPort.println("Select network by number: ");
    int choice = serialPort.get_int();

    String ssid;
    if (choice == 0) {
        bool confirmed = false;
        while (!confirmed) {
            serialPort.println("Enter network name: ");
            ssid = serialPort.get_string();
            serialPort.println("Confirm network name: ");
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
        serialPort.println("Enter network password: ");
        pwd = serialPort.get_string();
        serialPort.println("Confirm network password: ");
        serialPort.println(pwd);
        pwd_ok = serialPort.get_confirmation();
    }
}

bool System::connect_wifi() {
    String ssid, pwd;

    if(!read_memory_wifi_credentials(&ssid, &pwd))
        enter_wifi_credentials();

    while (!wifi_connected) {
        serialPort.print("Connecting to ");
        serialPort.println(ssid);

        // 4) Attempt connect
        if (wifi.connect(ssid, pwd)) {
            serialPort.print("Connected to ");
            serialPort.println(ssid);
            serialPort.print("Local ip: ");
            serialPort.println(wifi.get_local_ip());

            memory.write_bit("wifi_flags", 0, 1); //set wifi config exists flag
            memory.write_str("wifi_name", ssid);
            memory.write_str("wifi_pass", pwd);
            wifi_connected = true;
            return true;
        }
        else {
            serialPort.print("Failed to connect to ");
            serialPort.println(ssid);
            serialPort.println("Let's try again.");
            enter_wifi_credentials();
        }
    }
    return false;  // never reached, but placates the compiler
}
