# qtac-core Library

Qt-free C++11 static library providing drop-in replacements for Qt types and
a complete hardware abstraction stack (FTDI, PSoC, PIC32CX, STM32/BugHopper V2).
Used by `TACDev.dll` and `qtac-app` to eliminate the Qt dependency from core logic.

## Directory Structure

```
source/library/
├── cmake/
│   └── QtacCommon.cmake       # Shared build settings (platform flags, output paths)
├── include/qtac/
│   ├── ByteArray.h            # QByteArray replacement (header-only)
│   ├── String.h               # QString replacement
│   ├── StringList.h           # QStringList replacement
│   ├── Variant.h              # QVariant replacement
│   ├── Map.h                  # QMap replacement (header-only template)
│   ├── List.h                 # QList replacement (header-only template)
│   ├── Pair.h                 # QPair replacement (header-only alias)
│   ├── Signal.h               # Lightweight callback (replaces Qt signals)
│   ├── AlpacaDevice.h         # Device registry base
│   ├── AlpacaScript.h         # Script engine (Qt-free port)
│   ├── FTDIDevice.h           # FTDI/TACLite device
│   ├── FTDIPlatformConfiguration.h
│   ├── PSOCDevice.h           # PSoC TAC device
│   ├── PSOCPlatformConfiguration.h
│   ├── PIC32CXDevice.h        # PIC32CX TAC device
│   ├── PIC32CXPlatformConfiguration.h
│   ├── STM32Device.h          # BugHopper V2 (HID/hidapi)
│   ├── STM32PlatformConfiguration.h
│   ├── Notification.h         # Notification value type
│   └── ...                    # Protocol stack headers (TACLite, TACPSOC, etc.)
└── src/
    ├── qtac_core.cpp          # Library version info
    ├── String.cpp
    ├── StringList.cpp
    ├── Variant.cpp
    ├── AlpacaDevice.cpp
    ├── FTDIDevice.cpp
    ├── FTDIPlatformConfiguration.cpp
    ├── PSOCDevice.cpp / PSOCPlatformConfiguration.cpp
    ├── PIC32CXDevice.cpp / PIC32CXPlatformConfiguration.cpp
    ├── STM32Device.cpp / STM32PlatformConfiguration.cpp
    ├── TACSTM32*.cpp          # STM32 protocol stack (Coder/Protocol/Command/DriveThread)
    ├── TACLite*.cpp           # FTDI protocol stack
    ├── TACPSOC*.cpp           # PSoC protocol stack
    ├── TACPIC32CX*.cpp        # PIC32CX protocol stack
    └── ...
```

## Building

`qtac-core` is built as part of the root CMake project — it cannot be built
standalone. See [`source/BUILD.md`](../BUILD.md) for full instructions.

```bat
cmake -S C:\ProdTools\qcom-test-automation-controller ^
      -B build\Release ^
      -DCMAKE_PREFIX_PATH=C:\Qt\6.11.1\msvc2022_64 ^
      -G Ninja -DCMAKE_BUILD_TYPE=Release

cmake --build build\Release --target qtac-core
```

### External dependencies (fetched automatically by CMake)

| Dependency | Source | Usage |
|------------|--------|-------|
| `hidapi` | FetchContent (GitHub) | HID transport for STM32/BugHopper V2 |
| `libserialport` | FetchContent (GitHub sigrokproject) | Serial port for PSoC/PIC32CX |
| `boost` | vendored in `third-party/` | `boost::json` for tcnf/devicelist parsing |

## Qt-free Types

### `qtac::ByteArray` (replaces `QByteArray`)
Header-only. Wraps `std::string` with Qt-compatible API: append, prepend, insert, remove, hex/base64 encoding, numeric conversions, split, search, and comparison.

### `qtac::String` (replaces `QString`)
UTF-8 string class wrapping `std::string`. Key features:
- **`arg()`** — Qt-style `%1`..`%9` placeholder substitution
- **`split()`** — split by character or string separator
- **`number()`** — static int/double-to-string conversion
- **`toInt()`, `toUInt()`, `toULongLong()`** — numeric parsing
- **`toUpper()`, `toLower()`, `simplified()`, `trimmed()`** — text transforms
- **`toLatin1()`, `toUtf8()`** — conversion to `ByteArray`
- **`fromStdWString()`** — Windows wide-string conversion
- **`startsWith()`, `endsWith()`, `contains()`, `indexOf()`** — search

### `qtac::StringList` (replaces `QStringList`)
Wraps `std::vector<String>`. Provides `join()`, `filter()`, append, insert, remove, contains, indexOf.

### `qtac::Variant` (replaces `QVariant`)
Uses `std::variant` internally. Stores: `bool`, `int`, `unsigned int`, `long long`, `unsigned long long`, `double`, `String`, `ByteArray`.

### `qtac::Map<K, V>` (replaces `QMap`)
Header-only template wrapping `std::map`. Provides `value()`, `insert()`, `remove()`, `contains()`, `keys()`, `values()`.

### `qtac::List<T>` (replaces `QList`)
Header-only template wrapping `std::vector`. Provides `append()`, `prepend()`, `insert()`, `removeAt()`, `at()`, `contains()`, `indexOf()`, `first()`, `last()`.

### `qtac::Notification`
Value type carrying a message, severity level, id, timestamp (`std::chrono::system_clock`), and occurrence count. Defined in `namespace qtac` to avoid collision with `winrt::Windows::UI::Notifications::Notification`.

## Design Principles

1. **API compatibility** — Method names and signatures match Qt equivalents for easy migration
2. **No Qt headers** — Zero Qt includes; pure C++11
3. **Header-only where possible** — Templates (Map, List, Pair) and ByteArray are header-only
4. **Namespace isolation** — Everything in `namespace qtac` to avoid conflicts during migration
5. **Minimal footprint** — Only implements methods actually used in the codebase
