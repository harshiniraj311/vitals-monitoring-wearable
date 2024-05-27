# Vitals Monitoring Smart Wearable Device

## Overview
This project is a smart wearable device that monitors heart rate and temperature, displaying the data on an OLED screen and transmitting it via Bluetooth. The device is built using an Arduino microcontroller, a pulse sensor, a DS18B20 temperature sensor, an OLED display, and a Bluetooth module.

## Components
- Arduino microcontroller (e.g., Arduino Uno)
- Pulse Sensor
- DS18B20 Temperature Sensor
- Adafruit SSD1306 OLED Display
- Bluetooth Module (e.g., HC-05 or HC-06)
- Connecting wires and breadboard

## Features
- Real-time heart rate monitoring
- Temperature measurement
- Display of heart rate and temperature on OLED screen
- Bluetooth transmission of data for remote monitoring

## Circuit Diagram
Connect the components as follows:
- Pulse Sensor:
  - Signal to A1
  - VCC to 5V
  - GND to GND
- DS18B20 Temperature Sensor:
  - Data to A0
  - VCC to 5V
  - GND to GND
- OLED Display:
  - SDA to A4
  - SCL to A5
  - VCC to 3.3V
  - GND to GND
- Bluetooth Module:
  - RX to pin 2 (via voltage divider if necessary)
  - TX to pin 3
  - VCC to 3.3V
  - GND to GND

## Installation
1. Clone this repository: `git clone https://github.com/yourusername/vitals-monitoring-wearable.git`
2. Open `smartwearable.ino` with the Arduino IDE.
3. Install the required libraries:
   - `SoftwareSerial`
   - `OneWire`
   - `DallasTemperature`
   - `Adafruit_GFX`
   - `Adafruit_SSD1306`
4. Upload the code to your Arduino board.

## Usage
1. Power the Arduino board.
2. The OLED display will show the initial message "SMART WEARABLE".
3. Heart rate and temperature data will be displayed on the OLED screen.
4. Data will be transmitted via Bluetooth and can be received by a Bluetooth-enabled device.

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments
- Inspired by various open-source wearable health monitoring projects.
- Libraries used: Adafruit GFX Library, Adafruit SSD1306 Library, OneWire Library, DallasTemperature Library, SoftwareSerial Library.

## Demo
<img src="https://github.com/harshiniraj311/vitals-monitoring-wearable/assets/155360804/965340de-1b34-4d2e-8340-5628a161363d" alt="vitals monitoring wearable" width="400" height="480">

