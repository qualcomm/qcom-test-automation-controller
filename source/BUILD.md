# Build and Run Guide — TACDev and qtac-app

## Prerequisites (Windows)

| Tool | Version | Notes |
|------|---------|-------|
| Visual Studio Build Tools | 2022 | MSVC x64 toolchain (`cl.exe`) |
| CMake | 3.22+ | Must be on `PATH` |
| Ninja | any | Must be on `PATH` |
| Qt | 6.9+ | Any `msvc2022_64` kit under `C:\Qt\`; or set `QTDIR` env var |
| FTDI CDM driver SDK | 2.12.36+ | See §FTDI Bootstrap below |

---

## Prerequisites (Linux)

| Tool | Version | Notes |
|------|---------|-------|
| GCC | 7+ | C++11 support required |
| CMake | 3.22+ | Must be on `PATH` |
| Ninja | any | Install via `sudo apt install ninja-build` |
| Qt | 6.9+ | GCC 64-bit component required (e.g., `Qt/6.10.2/gcc_64`) |
| libudev-dev | any | Install via `sudo apt install libudev-dev` (required for hidapi) |

---

## Build (Linux)

### Using the build script (recommended)

The root-level `build.sh` script builds both Debug and Release configurations and deploys Qt runtime dependencies:

```bash
# Set QTBIN to point to your Qt installation's bin directory
export QTBIN=/path/to/Qt/6.10.2/gcc_64/bin

# Run the build script
./build.sh
```

The script will:
1. Validate that `QTBIN` points to a valid Qt GCC 64-bit installation
2. Download and extract the FTDI library (if not already present)
3. Configure and build Debug configuration
4. Configure and build Release configuration
5. Deploy Qt runtime libraries and plugins to `__Builds/Linux/Release/`

Outputs:
```
__Builds/Linux/Release/bin/qtac-app           # GUI application
__Builds/Linux/Release/lib/libTACDev.so       # Shared library
__Builds/Linux/Debug/lib/libqtac-core.a       # Static library
```

### Manual CMake steps

If you prefer manual control or want to build only Debug or Release:

```bash
# Set up environment
export QTBIN=/path/to/Qt/6.10.2/gcc_64/bin
export PATH="$QTBIN:$PATH"

# Configure Debug build
cmake -S source -B build/Debug \
    -DCMAKE_PREFIX_PATH="$(dirname "$QTBIN")" \
    -DCMAKE_BUILD_TYPE=Debug \
    -G Ninja

# Build Debug
cmake --build build/Debug

# Configure Release build
cmake -S source -B build/Release \
    -DCMAKE_PREFIX_PATH="$(dirname "$QTBIN")" \
    -DCMAKE_BUILD_TYPE=Release \
    -G Ninja

# Build Release
cmake --build build/Release
```

### Building individual targets

```bash
cmake --build build/Release --target qtac-app      # GUI application only
cmake --build build/Release --target TACDev        # Shared library only
cmake --build build/Release --target qtac-core     # Qt-free static lib only
```

### Incremental builds

After the initial configuration, you can rebuild quickly:

```bash
cd build/Debug   # or build/Release
make -j$(nproc)
```

---

## Running Tests (Linux)

### Non-hardware tests (no device required)

```bash
cd build/Debug/test

./test_bytearray
./test_coders_commands
./test_containers
./test_platform_configs
./test_signal
./test_string
./test_stringutils
./test_tacdev_api  # partial - init/version tests only
```

### Hardware integration tests (device must be connected)

```bash
cd build/Debug/test

./test_hardware_ftdi    # requires a TACLite (FTDI) device
./test_hardware_psoc    # requires a PSoC TAC device
./test_tacdev_api       # full test requires any supported TAC device
```

**Note**: Hardware tests require appropriate udev rules for device access. See `udev-rules/` directory.

---

## FTDI Bootstrap (automatic)

The FTDI `ftd2xx` library is downloaded and extracted automatically during
`cmake` configure. No manual setup is required. On first configure, CMake
fetches the CDM driver archive from the FTDI website (or the Wayback Machine
mirror), extracts `ftd2xx.lib` and `ftd2xx.dll`, and places them under
`__Builds\x64\` (Windows) or `__Builds\Linux\` (Linux).

If your machine has no internet access, manually place the archive at
`third-party\CDM-v2.12.36.4-WHQL-Certified.zip` before running cmake; the
bootstrap will use the local file instead of downloading it.

---

## Build (Windows)

### Quick build (recommended) — PowerShell

```powershell
cd C:\ProdTools\qcom-test-automation-controller
.\build_app.ps1
```

Performs:
1. Locates VS2022 via `vswhere.exe` and loads the MSVC x64 environment
2. Locates Qt: uses `QTDIR` env var if set, otherwise searches `C:\Qt\` for any `msvc2022_64` kit
3. `cmake -S source -B build\Release -DCMAKE_PREFIX_PATH=<Qt> -G Ninja -DCMAKE_BUILD_TYPE=Release`
4. `cmake --build build\Release --target qtac-app`
5. `cmake --build build\Release --target TACDev`

To use a non-default Qt install:
```powershell
$env:QTDIR = 'C:\Qt\6.11.1\msvc2022_64'
.\build_app.ps1
```

Outputs:
```
build\Release\qtac-app.exe
build\Release\TACDev.dll
build\Release\TACDev.lib   (import lib for callers)
```

### Quick build — batch file

```bat
cd C:\ProdTools\qcom-test-automation-controller
build_app.bat
```

Same steps as the PowerShell script; build log is saved to `build_out.txt`.

### Manual CMake steps

```bat
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
set PATH=C:\Qt\6.11.1\msvc2022_64\bin;%PATH%

cd C:\ProdTools\qcom-test-automation-controller

cmake -S source -B build\Release ^
      -DCMAKE_PREFIX_PATH=C:\Qt\6.11.1\msvc2022_64 ^
      -G Ninja ^
      -DCMAKE_BUILD_TYPE=Release

cmake --build build\Release --target qtac-app TACDev
```

### Building individual targets

```bat
cmake --build build\Release --target qtac-app      # GUI application only
cmake --build build\Release --target TACDev        # DLL only
cmake --build build\Release --target qtac-core     # Qt-free static lib only
```

---

## Rebuild (Windows — incremental, no reconfigure)

When only source files change and the build is already configured:

```powershell
cd C:\ProdTools\qcom-test-automation-controller
.\rebuild_library.ps1    # rebuilds qtac-core + TACDev + qtac-app (Debug)
.\rebuild_tests.ps1      # rebuilds test_coders_commands, test_hardware_ftdi, test_hardware_psoc (Debug)
```

---

## Deploy (Windows — qtac-app.exe)

After building, run the deploy script to copy Qt runtime DLLs and device
configuration files alongside the executable:

```powershell
cd C:\ProdTools\qcom-test-automation-controller
.\deploy_app.ps1
```

This copies into `build\Release\`:
- `ftd2xx.dll` — FTDI runtime
- `devicelist.json` — device catalogue
- `*.tcnf` — device configuration files
- `DefaultScript.txt` — default AlpacaScript
- All Qt DLLs/plugins (via `windeployqt`)

### Running qtac-app

```bat
build\Release\qtac-app.exe
```

The application expects `devicelist.json` and `.tcnf` files to be **in the
same directory** as the executable. `deploy_app.ps1` places them there.

---

## Deploy (Windows — TACDev.dll)

`TACDev.dll` is a Qt-free C API DLL. Consumers need:

| File | Location after build |
|------|---------------------|
| `TACDev.dll` | `build\Release\TACDev.dll` |
| `TACDev.lib` | `build\Release\source\tacdev\TACDev.lib` |
| `TACDev.h` | `source\tacdev\TACDev.h` |
| `ftd2xx.dll` | copy from `__Builds\x64\Release\bin\` |
| `devicelist.json` | `configurations\devicelist.json` |
| `*.tcnf` | `configurations\*.tcnf` |

**At runtime**, `devicelist.json` and all `.tcnf` files must be findable
relative to the executable that loads `TACDev.dll`. The DLL searches starting
from the exe directory and walking up to four levels.

### Minimal C usage example

```c
#include "TACDev.h"
#include <stdio.h>

int main(void) {
    if (InitializeTACDev() != NO_TAC_ERROR) return 1;

    int count = 0;
    GetDeviceCount(&count);
    printf("Devices found: %d\n", count);

    for (int i = 0; i < count; i++) {
        char port[256];
        GetPortData(i, port, sizeof(port));
        printf("  [%d] %s\n", i, port);
    }

    TAC_HANDLE h = OpenHandleByDescription("COM3");  // adjust as needed
    if (h == kBadHandle) { printf("Open failed\n"); return 1; }

    char name[128];
    GetName(h, name, sizeof(name));
    printf("Device: %s\n", name);

    SendCommand(h, "PowerKey", true);

    CloseTACHandle(h);
    return 0;
}
```

---

## Running Tests (Windows)

### Non-hardware tests (no device required)

```bat
cd C:\ProdTools\qcom-test-automation-controller\build\Release
test_bytearray.exe
test_coders_commands.exe
test_containers.exe
test_platform_configs.exe
test_signal.exe
test_string.exe
test_stringutils.exe
```

### Hardware integration tests (device must be connected)

```bat
cd C:\ProdTools\qcom-test-automation-controller\build\Release
test_hardware_ftdi.exe    # requires a TACLite (FTDI) device
test_hardware_psoc.exe    # requires a PSoC TAC device
test_tacdev_api.exe       # requires any supported TAC device
```

### Rebuild tests only

```powershell
cd C:\ProdTools\qcom-test-automation-controller
.\rebuild_tests.ps1
```

---

## Project Structure (relevant to this guide)

```
C:\ProdTools\qcom-test-automation-controller\
├── CMakeLists.txt              root — builds src/ subtree only (upstream)
├── source\
│   ├── CMakeLists.txt          standalone entry point for source/ subtree
│   ├── library\                qtac-core Qt-free static lib
│   ├── libraries\qt-adapter\   Qt ↔ qtac-core bridge (static lib)
│   ├── app\                    qtac-app Qt6 GUI source
│   ├── tacdev\                 TACDev.dll C API source + TACDev.h
│   └── test\                   unit + integration tests
├── build_app.ps1               PowerShell build script (uses vswhere)
├── build_app.bat               Batch build script
├── rebuild_library.ps1         Incremental rebuild of library + app targets
├── rebuild_tests.ps1           Incremental rebuild of test targets
├── deploy_app.ps1              windeployqt + copy configs
├── __Builds\x64\               ftd2xx.lib / ftd2xx.dll (auto-bootstrapped)
├── configurations\             devicelist.json, *.tcnf
├── third-party\                boost, hidapi, libserialport, nlohmann/json
└── build\Release\              cmake build output
```

---

## Troubleshooting (Windows)

| Symptom | Likely cause | Fix |
|---------|-------------|-----|
| `cmake` can't find Qt | Qt not on `CMAKE_PREFIX_PATH` | Pass `-DCMAKE_PREFIX_PATH=C:\Qt\6.11.1\msvc2022_64` |
| `ftd2xx.lib not found` | FTDI bootstrap failed or no internet | Delete `build\Release` and reconfigure; or pre-place archive at `third-party\CDM-v2.12.36.4-WHQL-Certified.zip` |
| `ftd2xx.dll not found` at runtime | DLL not deployed | Run `deploy_app.ps1` or copy manually |
| App opens but shows no devices | `devicelist.json` missing | Run `deploy_app.ps1` or copy `configurations\` files |
| `cl.exe` not found | MSVC env not loaded | Use `build_app.ps1`/`build_app.bat` which load `vcvars64.bat` |
| Build fails with `LINK : fatal error LNK1181` | Stale build dir | Delete `build\Release` and reconfigure |
| hidapi not found | FetchContent network issue | Ensure internet access during first configure |

---

## Troubleshooting (Linux)

| Symptom | Likely cause | Fix |
|---------|-------------|-----|
| `QTBIN is not set` | Environment variable missing | `export QTBIN=/path/to/Qt/6.x.x/gcc_64/bin` |
| `libudev not found` | Missing development headers | `sudo apt install libudev-dev` |
| `cmake` can't find Qt | Qt not on `CMAKE_PREFIX_PATH` | Pass `-DCMAKE_PREFIX_PATH=/path/to/Qt/6.x.x/gcc_64` |
| `ninja not found` | Ninja build tool not installed | `sudo apt install ninja-build` |
| Permission denied on device | Missing udev rules | Copy rules from `udev-rules/` to `/etc/udev/rules.d/` and reload |
| Qt platform plugin error | Missing Qt XCB libraries | Ensure `libQt6XcbQpa.so` is deployed (done by `build.sh`) |
| hidapi not found | FetchContent network issue | Ensure internet access during first configure |
| Ambiguous `String::number()` call | uint64_t type mismatch | Update to `static_cast<unsigned long long>` |
