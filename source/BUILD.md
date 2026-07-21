# Build and Run Guide — TACDev.dll and qtac-app.exe

## Prerequisites

| Tool | Version | Notes |
|------|---------|-------|
| Visual Studio Build Tools | 2022 | MSVC x64 toolchain (`cl.exe`) |
| CMake | 3.16+ | Must be on `PATH` |
| Ninja | any | Must be on `PATH` |
| Qt | 6.11.1 | Installed to `C:\Qt\6.11.1\msvc2022_64\` |
| FTDI CDM driver SDK | 2.12.36+ | See §FTDI Bootstrap below |

---

## FTDI Bootstrap (one-time)

The build requires `ftd2xx.lib` and `ftd2xx.dll` placed under `__Builds\x64\`.

### Option A — Extract from the vendored zip (no separate repo needed)

```powershell
cd C:\ProdTools\qtac-refactor
.\extract_ftdi.ps1
```

This extracts `third-party\CDM-v2.12.36.20-WHQL-Certified.zip` and copies:

```
__Builds\x64\Debug\lib\ftd2xx.lib
__Builds\x64\Debug\bin\ftd2xx.dll
__Builds\x64\Release\lib\ftd2xx.lib
__Builds\x64\Release\bin\ftd2xx.dll
```

### Option B — Copy from an existing `qcom-test-automation-controller` checkout

```powershell
cd C:\ProdTools\qtac-refactor
.\copy_ftdi.ps1
```

Copies the same four files from `C:\ProdTools\qcom-test-automation-controller\__Builds\x64\`.

---

## Build

### Quick build (recommended) — PowerShell

```powershell
cd C:\ProdTools\qtac-refactor
.\build_app.ps1
```

Performs:
1. Loads the MSVC x64 environment from `vcvars64.bat`
2. Adds Qt `bin\` to `PATH`
3. `cmake -S . -B build\Release -DCMAKE_PREFIX_PATH=<Qt> -G Ninja -DCMAKE_BUILD_TYPE=Release`
4. `cmake --build build\Release --target qtac-app`
5. `cmake --build build\Release --target TACDev`

Outputs:
```
build\Release\qtac-app.exe
build\Release\TACDev.dll
build\Release\TACDev.lib   (import lib for callers)
```

### Quick build — batch file

```bat
cd C:\ProdTools\qtac-refactor
build_app.bat
```

Same steps as the PowerShell script; build log is saved to `build_out.txt`.

### Manual CMake steps

```bat
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
set PATH=C:\Qt\6.11.1\msvc2022_64\bin;%PATH%

cd C:\ProdTools\qtac-refactor

cmake -S . -B build\Release ^
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

## Deploy (qtac-app.exe)

After building, run the deploy script to copy Qt runtime DLLs and device
configuration files alongside the executable:

```powershell
cd C:\ProdTools\qtac-refactor
.\deploy_app.ps1
```

This copies into `build\Release\`:
- `ftd2xx.dll` — FTDI runtime
- `devicelist.json` — device catalogue
- `*.tcnf` — device configuration files
- All Qt DLLs/plugins (via `windeployqt`)

### Running qtac-app

```bat
build\Release\qtac-app.exe
```

The application expects `devicelist.json` and `.tcnf` files to be **in the
same directory** as the executable. `deploy_app.ps1` places them there.

---

## Deploy (TACDev.dll)

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

## Running the TACDev API Test

A standalone test binary exercises the C API against a live device.

```bat
cd C:\ProdTools\qtac-refactor
build_tacdev_test.bat
```

Or manually:

```bat
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

cmake -S source\test -B build_test_api -G Ninja -DCMAKE_BUILD_TYPE=Release --fresh
cmake --build build_test_api --target test_tacdev_api

build_test_api\test_tacdev_api.exe
```

---

## Project Structure (relevant to this guide)

```
C:\ProdTools\qtac-refactor\
├── CMakeLists.txt              root — adds all subdirectories
├── build_app.ps1               PowerShell build script
├── build_app.bat               Batch build script
├── deploy_app.ps1              windeployqt + copy configs
├── copy_ftdi.ps1               copy FTDI files from sibling repo
├── extract_ftdi.ps1            extract FTDI files from vendored zip
├── __Builds\x64\               ftd2xx.lib / ftd2xx.dll (bootstrapped)
├── configurations\             devicelist.json, *.tcnf
├── third-party\                nlohmann/json, libserialport, FTDI zip
├── source\
│   ├── library\                qtac-core (Qt-free static lib)
│   ├── libraries\qt-adapter\   Qt ↔ qtac-core bridge (static lib)
│   ├── app\                    qtac-app Qt6 GUI source
│   ├── tacdev\                 TACDev.dll C API source + TACDev.h
│   └── test\                   unit + integration tests
└── build\Release\              cmake build output
```

---

## Troubleshooting

| Symptom | Likely cause | Fix |
|---------|-------------|-----|
| `cmake` can't find Qt | Qt not on `CMAKE_PREFIX_PATH` | Pass `-DCMAKE_PREFIX_PATH=C:\Qt\6.11.1\msvc2022_64` |
| `ftd2xx.lib not found` | FTDI not bootstrapped | Run `extract_ftdi.ps1` or `copy_ftdi.ps1` |
| `ftd2xx.dll not found` at runtime | DLL not deployed | Run `deploy_app.ps1` or copy manually |
| App opens but shows no devices | `devicelist.json` missing | Run `deploy_app.ps1` or copy `configurations\` files |
| `cl.exe` not found | MSVC env not loaded | Use `build_app.ps1`/`build_app.bat` which load `vcvars64.bat` |
| Build fails with `LINK : fatal error LNK1181` | Stale build dir | Delete `build\Release` and reconfigure |
