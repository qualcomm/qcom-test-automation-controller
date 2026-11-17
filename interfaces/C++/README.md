# TAC C++ Interface (TACDev)

`TACDev` is the native C/C++ interface library for the Qualcomm Test Automation Controller (QTAC). It provides programmatic control over Qualcomm debug boards to automate hardware testing, device rebooting, GPIO pin toggling, power cycling, and boot mode transitions (FastBoot, EDL, UEFI).

---

## Directory Contents

- **`TACDev.h`**: Public C/C++ header defining all exported functions, handles, and result codes.
- **`TACDev.cpp` / `TACDevCore.cpp`**: Shared library implementation wrapping `qcommon-console`.
- **`CMakeLists.txt`**: CMake target definition for building `TACDev.dll` (Windows) or `libTACDev.so` (Linux).

---

## Complete API Documentation

For the full class and function reference, error codes, parameter definitions, and usage examples, please refer to:

👉 **[QTAC C++ API Documentation](../../docs/bootcamp/04-CPP-API.md)**

---

## Quick Example

```cpp
#include "TACDev.h"
#include <iostream>

int main()
{
    // Initialize library
    if (InitializeTACDev() != NO_TAC_ERROR) return 1;

    // Check device count
    int count = 0;
    GetDeviceCount(&count);
    std::cout << "Connected TAC devices: " << count << std::endl;

    if (count > 0)
    {
        // Open device (by port or serial number)
        TAC_HANDLE handle = OpenHandleByDescription("COM13");
        if (handle != kBadHandle)
        {
            // Transition target device to EDL mode
            BootToEDLButton(handle);

            // Close handle
            CloseTACHandle(handle);
        }
    }

    return 0;
}
```

---

## Building

The C++ interface can be built either:
1. As part of the full QTAC build via `build.bat` / `build.sh`.
2. As part of the standalone **CLI-only** build without Qt dependencies. See [CLI Documentation](../../cli/README.md).
