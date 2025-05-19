/*
purpose: this is the main class that orchistrates everything.


*/

#include "../Interfaces/TerminalInterface/TerminalInterface.h"
#include "../Wifi/Wifi.h"


class System {


    void init_system_setup(){
        terminal.println("Welcome to the XeWe Led OS!");
        terminal.println("This is a complete OS solution to control the addressable LED lights.");
        terminal.println("Communication supported: serial port");
        terminal.println("Communication to be supported: Webserver, Alexa, Homekit, Yandex-Alisa");

        terminal.println("Lets connect to WiFi:");
        connect_wifi();
    }

    bool connect_wifi(){
        wifi_connected = false;
        while(!wifi_connected){
            String * wifi_networks = wifi.get_available_networks();
            terminal.println(wifi_networks);

            int network_selected_id = terminal.get_int();
            String selected_network_name = "";
            if(network_selected_id = 0){
                boolean network_name_selected = false;
                while(!network_name_selected){
                    terminal.print("Enter network name:")
                    selected_network_name = terminal.get_string();
                    terminal.print("Confirm network name:")
                    terminal.print(selected_network_name)
                    network_name_selected = terminal.get_confirmation();
                }
            }

            String network_pass = "";
            boolean network_pass_entered = false;
            while(!network_pass_entered){
                terminal.print("Enter network password:")
                network_pass = terminal.get_string();
                terminal.print("Confirm network password:")
                terminal.print(network_pass)
                network_pass_entered = terminal.get_confirmation();
            }

            wifi_connected = wifi.connect(selected_network_name, network_pass);

            if (wifi_connected){
                terminal.print("Connected to ")
                terminal.print(selected_network_name)
                memory.write("wifi_name", selected_network_name);
                memory.write("wifi_pass", network_pass);
                return true;
            } else {
                terminal.print("There was an issue connecting to ")
                terminal.print(selected_network_name)
                terminal.print("Let's try again")
            }
        }
    }


};