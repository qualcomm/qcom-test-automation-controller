# QTAC Command-Line Interface (CLI Only)

This directory provides a lightweight, standalone build configuration for the QTAC command-line interface (`TACTerminal`) and the C++ interface library (`TACDev`) **without any dependency on Qt**.

This build is ideal for headless environments, automated continuous integration (CI/CD) pipelines, test racks, Docker containers, and developers who do not require the Qt GUI.

---

## What Gets Built

| Component | Type | Description |
| :--- | :--- | :--- |
| **`TACTerminal`** | Executable | Command-line utility for device discovery, querying status, sending platform commands, and controlling boot modes. |
| **`TACDev`** | Shared Library (`.dll` / `.so`) | The official C/C++ interface library for integrating TAC control into your own C/C++ applications. |
| **`QCommonConsole`** | Static Library | Core library providing device drivers (FTDI, Cypress PSoC, Microchip PIC32CX, STM32), protocol parsers, and platform configurations. |

---

## Prerequisites

Building the CLI-only version requires **only standard C++ tools** — **Qt is NOT required**.

| Tool | Windows | Linux | Minimum Version |
| :--- | :--- | :--- | :--- |
| **Compiler** | MSVC 2022 (Desktop C++) | GCC / G++ or Clang | C++20 support (MSVC 19.30+ / GCC 11+) |
| **Build System** | [CMake](https://cmake.org/download/) | CMake (`sudo apt install cmake`) | 3.22+ |
| **Build Tool** | MSBuild / Ninja | Ninja or Make (`sudo apt install ninja-build`) | 1.10+ |
| **Drivers / Libs** | FTDI D2XX (auto-downloaded) | `libudev-dev` (`sudo apt install libudev-dev`) | - |

> [!NOTE]
> On both Windows and Linux, the FTDI D2XX library is automatically fetched and configured by CMake during project setup.

---

## Build Instructions

### Windows (Visual Studio 2022)

Open a **Developer Command Prompt for VS 2022** or PowerShell:

```cmd
# 1. Configure the project
cmake -S cli -B cli/build -G "Visual Studio 17 2022" -A x64

# 2. Build Release configuration
cmake --build cli/build --config Release
```

Or using **Ninja**:

```cmd
cmake -S cli -B cli/build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build cli/build
```

### Linux (Ubuntu / Debian)

Install dependencies and build:

```bash
# 1. Install prerequisites
sudo apt update
sudo apt install build-essential cmake ninja-build libudev-dev

# 2. Configure the project
cmake -S cli -B cli/build -G Ninja -DCMAKE_BUILD_TYPE=Release

# 3. Build
cmake --build cli/build
```

---

## Output Binaries

Built executables and libraries are located in:

- **Windows**: `cli/build/bin/Release/` (or `cli/__Builds/x64/Release/bin/`)
  - `TACTerminal.exe`
  - `TACDev.dll`
- **Linux**: `cli/build/bin/` (or `cli/__Builds/Linux/Release/bin/`)
  - `TACTerminal`
  - `libTACDev.so`

---

## TACTerminal Usage Examples

`TACTerminal` provides a command-line interface to interact with connected Qualcomm debug boards:

### 1. Discover Connected Devices
```bash
# List all connected TAC devices
TACTerminal --list-devices

# Print the number of connected devices
TACTerminal --count
```

### 2. View Device Information
```bash
# Query hardware, firmware, and UUID (targets first connected device if -d omitted)
TACTerminal -d COM13 --info
# Linux:
TACTerminal -d /dev/ttyUSB0 --info
```

### 3. List Supported Platform Commands
```bash
TACTerminal -d COM13 --list-commands
```

### 4. Send Commands & Boot Modes
```bash
# Boot target device to EDL mode
TACTerminal -d COM13 -c boottoedl

# Boot target device to FastBoot mode
TACTerminal -d COM13 -c boottofastboot

# Toggle power key
TACTerminal -d COM13 -c powerkey
```

### 5. Version Information
```bash
TACTerminal --version
```
