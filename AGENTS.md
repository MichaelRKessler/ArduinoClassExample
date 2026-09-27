# AGENTS.md

Instructions and workflow guide for AI agents working in this repository.

---

## 1. Project Overview & Workflow Purpose

This repository is designed for Arduino microcontroller firmware development with a streamlined two-phase workflow:
1. **Simulation Phase (VS Code + Wokwi Simulator)**: Develop, debug, and simulate circuit behavior and firmware directly within Visual Studio Code using the **Wokwi** simulator extension (commonly referred to as "Walkie" / Wokwi).
2. **Hardware Deployment Phase (Arduino CLI)**: Use the **Arduino CLI** (`arduino-cli`) to compile binaries into a local `build/` directory, install cores/libraries, detect connected boards, and upload/flash the firmware directly to the microcontroller over the command line.

---

## 2. Repository Layout & Conventions

To ensure seamless compatibility between Wokwi simulation, local compilation, and the Arduino CLI, follow this directory layout:

```text
ArduinoClassExample/
├── .gitignore                 # Excludes build/ binaries and temp files from git
├── AGENTS.md                  # Instructions and workflow rules for AI agents
├── diagram.json               # Wokwi simulation circuit layout and wiring
├── wokwi.toml                 # Wokwi configuration file pointing to local build artifacts
├── libraries.txt              # (Optional) External Arduino libraries for Wokwi simulation
├── ArduinoClassExample.ino    # Main Arduino sketch file (entry point)
├── src/                       # Modular C++ source and header files (.cpp / .h)
└── build/                     # Local output directory for compiled binaries (.hex, .elf)
```

> **Local Build Directory (`build/`)**: Compiled binaries (`.hex`, `.elf`, `.bin`) are directed into `./build/` so Wokwi can simulate the exact compiled output, while keeping the repository root clean and ignoring generated binaries in `.gitignore`.

---

## 3. Toolchain & Workflow Guidelines

### Phase A: Local Build & VS Code Simulation with Wokwi ("Walkie")

1. **Prerequisites**:
   - VS Code with the **Wokwi Simulator** extension (`wokwi.vscode-wokwi`) installed.
   - Arduino CLI installed (to compile binaries into `./build/`).
2. **Key Wokwi Configuration Files**:
   - **`wokwi.toml`**:
     Points Wokwi directly to the local compilation output in `build/`:
     ```toml
     [wokwi]
     version = 1
     firmware = "build/ArduinoClassExample.ino.hex"
     elf = "build/ArduinoClassExample.ino.elf"
     ```
     *(Note: For ESP32 or ARM boards, `firmware` will point to `.bin` instead of `.hex`).*
   - **`diagram.json`**:
     Defines the microcontroller (e.g., Arduino Uno `wokwi-arduino-uno`, Nano `wokwi-arduino-nano`, ESP32 `wokwi-esp32-devkit-v1`), connected peripherals (LEDs, resistors, sensors, LCDs), and wire connections.
3. **Compile for Wokwi Simulation**:
   Generate the simulation binaries in the local `build/` folder:
   ```powershell
   arduino-cli compile --fqbn arduino:avr:uno --build-path ./build .
   ```
4. **Run Simulation**:
   - Open the command palette in VS Code (`Ctrl+Shift+P` / `Cmd+Shift+P`).
   - Run `Wokwi: Start Simulator` or press the play button when viewing `diagram.json`.
   - Monitor the Serial Monitor output directly in the VS Code terminal / Wokwi console.

---

### Phase B: Hardware Deployment with Arduino CLI (`arduino-cli`)

When ready to flash the firmware to real hardware:

#### 1. Setup & Core Installation (One-time or as needed)
Update the core index and ensure the core for your target microcontroller is installed:
```powershell
# Update package index
arduino-cli core update-index

# For Arduino Uno, Nano, Mega (AVR architecture):
arduino-cli core install arduino:avr

# For ESP32 boards (if applicable):
# arduino-cli core install esp32:esp32
```

#### 2. Install Required Libraries
```powershell
# Search for a library
arduino-cli lib search "<LibraryName>"

# Install a library
arduino-cli lib install "<LibraryName>"
```

#### 3. Detect Connected Microcontroller & Port
Connect your microcontroller via USB and run:
```powershell
arduino-cli board list
```
Note the detected **Port** (e.g., `COM3`, `/dev/ttyACM0`) and the **FQBN** (Fully Qualified Board Name, e.g., `arduino:avr:uno`, `arduino:avr:nano`).

#### 4. Compile the Sketch to `build/`
Compile locally to generate the binary artifacts in `build/`:
```powershell
arduino-cli compile --fqbn arduino:avr:uno --build-path ./build .
```

#### 5. Upload / Flash Firmware
Flash the compiled binary to the microcontroller:
```powershell
# Flash directly from the sketch specifying build path:
arduino-cli upload -p COM3 --fqbn arduino:avr:uno --input-dir ./build

# Or compile and upload in one step:
arduino-cli compile --upload -p COM3 --fqbn arduino:avr:uno --build-path ./build .
```
*(Replace `COM3` with your active port and `arduino:avr:uno` with your target board's FQBN).*

#### 6. Open Serial Monitor via CLI
To read runtime output directly from the terminal:
```powershell
arduino-cli monitor -p COM3 -c baudrate=115200
```
*(Use `Ctrl+C` to exit the monitor).*

---

## 4. Coding & Agent Guidelines

When creating, refactoring, or editing code in this repository, agents must adhere to the following rules:

1. **Local Build Routing**:
   - Always compile using `--build-path ./build` so generated binaries do not clutter the source tree and Wokwi can locate `build/ArduinoClassExample.ino.hex` / `.elf`.
   - Never commit files inside `build/` to version control.
2. **Non-blocking Timing**:
   - Avoid long `delay()` calls in the main `loop()`. Prefer `millis()`-based state machines or non-blocking intervals to maintain responsiveness.
3. **Explicit Pin & Constant Definitions**:
   - Use `constexpr uint8_t` or descriptive `#define` macros for all pin numbers at the top of the sketch or in dedicated headers.
   - Ensure pin assignments in code strictly match the connections declared in `diagram.json`.
4. **Serial Communication**:
   - Always initialize `Serial.begin(115200);` (or consistent baud rate) in `setup()`.
   - Provide informative debug messages for setup completion and major state changes.
5. **Modular Architecture**:
   - Place reusable logic, drivers, and state machines into `src/` as separate `.h` and `.cpp` files to keep the main `.ino` sketch concise.
6. **Circuit Synchronization**:
   - Whenever introducing new electronic components (buttons, LEDs, sensors, displays), update both the code pin mapping AND `diagram.json` so the simulation remains fully functional.
