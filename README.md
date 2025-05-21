# Welcome to XeWe Led OS

## Quickstart

### Download Arduino IDE
  - For Mac users, you need to install the **Intel** version, **APPLE SILICON** VERSION DOES NOT WORK WITH ESP32 BOARDS
  - Launch IDE
  - Go to settings
  - Set Additional boards manager URLs: http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Close IDE

### Connect ESP32 C3 to USB

### Open [XeWe-LedOS.ino](XeWe-LedOS.ino) sketch
  - Select board ESP32C3 Dev Module
  - Select your port 
  - Go to Tools -> USB CDC On Boot -> "Enabled"
  - In the sketch:
    - Make sure you have ```#define LED_PIN <your_led_strip_pin>```
    - Verify ```#define NUM_LEDS <your_led_strip_led_count>```
  - Press upload

### To communicate with the board, you can use Serial Port
  - Go to Tools -> Serial Monitor
  - Set 115200baud rate
  - Disconnect and connect the board to reload it

### Connect to WiFi
  - WiFi connection will be prompted automatically
  - Select your WiFi network using the number
  - Enter password
  - Done!
  - To see wifi commands available type ```$wifi help```

### Control LED Strip
  - If it is your first startup type ```$led reset``` to reset the EEPROM memory
  - Type ```$led help``` to see commands available
  - All the parameters have the range 0-255: ```$led set_brightness <0-255>```
  - All the parameters must be separated with a space: ```$led set_rgb <0-255> <0-255> <0-255>```
  - Example: 
    - Set led to red: ```$led set_rgb 255 0 0```
    - Set brightness to 50%: ```$led set_brightness 127```
    - Turn on: ```$led turn_on```
    - 