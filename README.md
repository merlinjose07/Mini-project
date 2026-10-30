# Electrical and Electronics Engineering – Mini Projects

This repository contains mini projects developed as part of my learning in Electrical and Electronics Engineering, embedded systems and practical hardware implementation.

## Projects

### 1. Smart Fire and Gas Detection System Using ESP32

An ESP32-based monitoring prototype using a DHT22, MQ-2 gas/smoke sensor and LDR light sensor. It provides local LED/buzzer indications, LCD readings and Blynk IoT monitoring/notifications.

- ESP32-based sensor monitoring
- Temperature and humidity measurement
- MQ-2 raw gas/smoke sensor readings
- LDR high-light indication
- Three-level status indication using LEDs
- LCD display and Blynk dashboard integration

**Project documentation:** [Smart Fire and Gas Detection README](smart-fire-and-gas-detection/README.md)  
**Source code:** [smart_fire_gas_detection.ino](smart-fire-and-gas-detection/Code/smart_fire_gas_detection.ino)

> Safety note: This is an educational prototype, not a certified fire or gas safety device. The LDR detects light intensity rather than confirming flame, and the MQ-2 value is an uncalibrated ADC reading.

### 2. Tic-Tac-Toe Game Using Arduino

A two-player game built using an Arduino UNO, 4×4 matrix keypad and 20×4 I2C LCD.

- Keypad-based player input
- LCD display of the game board and current turn
- Win and draw detection
- Restart function

**Source code:** [tic_tac_toe.ino](tic-tac-toe-arduino/Code/tic_tac_toe.ino)

## Repository Structure

- `smart-fire-and-gas-detection/` – ESP32 fire and gas monitoring prototype
- `tic-tac-toe-arduino/` – Arduino keypad-based Tic-Tac-Toe game

---
*Academic mini projects – Electrical and Electronics Engineering.*
