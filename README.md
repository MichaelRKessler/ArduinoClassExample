# ArduinoClassExample

An Arduino firmware project demonstrating non-blocking microcontroller programming, VS Code Wokwi simulation, and command-line hardware deployment via the Arduino CLI.

## Overview

This project reads distance from an HC-SR04 ultrasonic sensor to control two outputs:
- **LED Pulse Rate**: The LED blinks faster the closer an object gets to the sensor (from 1000 ms interval at 100 cm down to 60 ms interval at 5 cm).
- **Servo Dial Gauge**: The servo motor sweeps from 0° (idle / far) to 180° (close / warning), acting as a physical proximity gauge.

All operations use non-blocking `millis()` timing to maintain real-time responsiveness.

---

## Hardware & Wiring

| Component | Pin | Connected To |
| :--- | :--- | :--- |
| **Ultrasonic Sensor (HC-SR04)** | VCC | 5V |
| | GND | GND |
| | TRIG | Digital Pin 2 |
| | ECHO | Digital Pin 3 |
| **Servo Motor** | V+ | 5V |
| | GND | GND |
| | PWM (Signal) | Digital Pin 9 |
| **LED** | Anode | Digital Pin 13 (via 220Ω resistor) |
| | Cathode | GND |

---

## Simulation with Wokwi (VS Code)

1. Install the **Wokwi Simulator** extension in Visual Studio Code (`wokwi.vscode-wokwi`).
2. Open `diagram.json` and press the **Play** button, or run **Wokwi: Start Simulator** (`Ctrl+Shift+P`).
3. Click on the HC-SR04 sensor in the simulation window and move the distance slider to observe the servo rotation and changing LED blink rate.

---

## Building and Flashing via Arduino CLI

### 1. Compile Locally
```powershell
arduino-cli compile --fqbn arduino:avr:uno --build-path ./build .
```

### 2. Detect Connected Arduino
```powershell
arduino-cli board list
```

### 3. Upload / Flash Firmware
```powershell
arduino-cli upload -p COM5 --fqbn arduino:avr:uno --input-dir ./build
```
*(Replace `COM5` with your active serial port).*

### 4. Serial Monitor
```powershell
arduino-cli monitor -p COM5 -c baudrate=115200
```
