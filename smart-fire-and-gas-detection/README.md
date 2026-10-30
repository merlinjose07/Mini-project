# Smart Fire and Gas Detection System Using ESP32

An ESP32-based academic monitoring prototype that reads temperature, humidity, gas/smoke sensor output and ambient light level. It provides local LED/buzzer indications, an I2C LCD display and remote monitoring/alert functions through the Blynk IoT platform.

> **Safety note:** This is an educational prototype, not a certified fire alarm or gas safety device. Do not rely on it for life-safety protection. The LDR in this code detects high light intensity and cannot reliably confirm a flame. The MQ-2 output is an uncalibrated ADC reading, not a gas concentration in ppm.

## Features

- DHT22 temperature and humidity monitoring
- MQ-2 gas/smoke sensor analog reading
- LDR-based high-light indication (not verified flame detection)
- Green, yellow and red status LEDs
- Buzzer alarm with Blynk mute control
- 16×2 I2C LCD with rotating display pages
- Blynk dashboard values and danger event notification
- Local sensing and alarm logic continue when Wi-Fi/Blynk is unavailable

## Hardware Used

- ESP32 development board
- DHT22 temperature and humidity sensor
- MQ-2 gas/smoke sensor module
- LDR/light sensor module with analog output
- 16×2 I2C LCD (code address: `0x27`)
- Green, yellow and red LEDs with suitable current-limiting resistors
- Buzzer
- Breadboard, jumper wires and suitable regulated power supply

## ESP32 Pin Configuration

| Module / Signal | ESP32 GPIO |
|---|---:|
| DHT22 data | GPIO 4 |
| MQ-2 analog output | GPIO 34 |
| LDR analog output | GPIO 32 |
| Green LED | GPIO 27 |
| Yellow LED | GPIO 26 |
| Red LED | GPIO 25 |
| Buzzer | GPIO 14 |
| LCD SDA | GPIO 21 |
| LCD SCL | GPIO 22 |

Connect all module grounds to a common ground. Verify each module's supply-voltage requirements and ensure every ESP32 input stays within the ESP32's permitted voltage range. Do not connect a 5 V analog output directly to an ESP32 GPIO.

## Blynk Virtual Pins

| Virtual pin | Data |
|---|---|
| V0 | Temperature (°C) |
| V1 | Humidity (%) |
| V2 | MQ-2 raw ADC reading |
| V3 | LDR raw ADC reading |
| V4 | High-light status flag |
| V5 | Alarm state: 0 normal, 1 warning, 2 danger |
| V6 | Buzzer mute switch (app input) |
| V7 | Status text |

The code calls the Blynk event `danger_alert`. Create this event in the Blynk template and configure the required notification settings.

## Software and Setup

1. Install Arduino IDE and the ESP32 board package.
2. Install the **Blynk** and **DHT sensor library** libraries, plus the required **Adafruit Unified Sensor** dependency if requested by the DHT library.
3. Install a compatible **LiquidCrystal_I2C** library.
4. Create a Blynk template and device. Copy its Template ID, Template Name and Device Auth Token into the three definitions at the top of the sketch.
5. For Wokwi, the sketch uses `Wokwi-GUEST` with an empty password. For physical hardware, replace these with your Wi-Fi credentials.
6. Check the LCD I2C address; change `0x27` if your module uses another address.
7. Upload the sketch to the ESP32 and configure Blynk datastreams V0–V7 and the `danger_alert` event.

## Thresholds

The current sketch uses these demonstration thresholds:

| Parameter | Warning | Danger |
|---|---:|---:|
| Temperature | 30 °C | 33 °C |
| Humidity | 75 % | 85 % |
| MQ-2 raw ADC | 3300 | 3800 |
| LDR raw ADC | 2500 | 3200 |

These are example thresholds from the project code, not validated safety limits. ADC values depend on the sensor module, supply, wiring, environment and ESP32 ADC characteristics. Calibrate and validate the prototype in a controlled educational setup; do not interpret raw ADC readings as calibrated gas concentration.

## LCD Pages

1. Temperature and humidity
2. MQ-2 gas ADC and LDR light ADC
3. Overall status: NORMAL, WARNING or DANGER

## Repository Structure

- `Code/smart_fire_gas_detection.ino` — ESP32 source sketch

## Limitations

- The LDR only senses light intensity; sunlight, lamps and reflections can cause false indications, and some flames may not produce a reliable LDR response.
- MQ-2 readings require warm-up, calibration and appropriate interpretation. This sketch does not calculate ppm or identify a specific gas.
- Thresholds are demonstration values and need calibration for the actual components.
- Network notifications depend on Wi-Fi and Blynk connectivity.
- The prototype is not a substitute for certified smoke, fire or combustible-gas detection equipment.

---
*Academic mini project – Electrical and Electronics Engineering.*
