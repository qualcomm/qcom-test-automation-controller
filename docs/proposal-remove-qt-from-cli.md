# Proposal: Remove Qt Dependency from QCommonConsole & TACDev Interface Library

**Date:** 2026-05-12
**Status:** Draft
**Scope:** QCommonConsole library, TACDev interface library only

---

## 1. Problem Statement

The two foundational libraries in the QTAC project — **QCommonConsole** (core device logic) and **TACDev** (public C/C++ API) — are tightly coupled to the Qt framework despite containing no GUI code. Every consumer of these libraries (CLI tools, automation scripts, language bindings) inherits the full Qt dependency chain.

| Problem | Impact |
|---------|--------|
| **Heavy runtime** | Any binary linking QCommonConsole ships ~50-100 MB of Qt DLLs (Core, SerialPort, Network, Xml, Concurrent) |
| **Build requirement** | Qt 6.9+ SDK must be installed to compile anything that touches device hardware |
| **Licensing burden** | Qt's LGPL/commercial license propagates to all consumers of the core library |
| **Platform lock-in** | Core library cannot be used on platforms where Qt is unavailable (embedded Linux, RTOS, minimal containers) |
| **Cross-compilation friction** | Cross-compiling Qt for ARM/other architectures is significantly harder than pure C++ |
| **MOC overhead** | Qt's Meta-Object Compiler adds a build step to every file with `Q_OBJECT`, slowing compilation |

**Goal:** `QCommonConsole` and `TACDev` should depend only on standard C/C++ (C++17/20) and minimal, vendorable libraries — usable by any consumer (CLI, GUI, scripts, embedded) without requiring Qt.

**Out of scope:** CLI applications (`devlist`, `tacdump`, `ftdi-check`, etc.), GUI applications, and UI libraries (`QCommon`, `UI-Common`). These consume the core library and will benefit automatically once it is Qt-free, but their own code is not changed in this proposal.

---

## 2. Current Qt Usage Inventory

### 2.1 Qt Modules Linked

| Library | Qt Modules | Why |
|---------|-----------|-----|
| **QCommonConsole** | Core, SerialPort, Network, Xml, Concurrent | Strings, containers, serial I/O, JSON, threading |
| **TACDev** | Core, SerialPort | Wraps QCommonConsole, passes Qt types through |

### 2.2 Qt Classes Used in QCommonConsole

**Strings & Buffers (~70 files)**

| Qt Class | Where Used | Role |
|----------|-----------|------|
| `QString` | Nearly every header and source file | Primary string type throughout the library |
| `QByteArray` | SerialPort, DriveThread, FrameCoder, FramePackage, FTDIChipset, AlpacaScript | Raw byte buffer for serial I/O and protocol framing |
| `QStringList` | CommandLineParser, AlpacaDevice, PlatformConfiguration | Lists of command names, port names, arguments |

**Containers (~40 files)**

| Qt Class | Where Used | Role |
|----------|-----------|------|
| `QList<T>` | AlpacaDevice, TACCommand, AlpacaScript, USBDescriptors, RecentFiles | Device lists, command lists, script variables |
| `QMap<K,V>` | CommandLineParser, TACCommand, AlpacaScript, DriveThread, PlatformConfiguration, TACDevCore | Command-name-to-object lookup, script variable storage, pin mapping |
| `QVector<T>` | DataPoints | Data point arrays |
| `QPair<T,U>` | ErrorParameter, FrameCoder | Error code pairs, frame fields |

**Serial Port (~10 files)**

| Qt Class | Where Used | Role |
|----------|-----------|------|
| `QSerialPort` | SerialPort.h/cpp (inherits from it) | All UART communication with PSoC and PIC32CX boards |
| `QSerialPortInfo` | SerialPort.h/cpp, CommDevice | Port enumeration and device discovery |

**JSON (~8 files)**

| Qt Class | Where Used | Role |
|----------|-----------|------|
| `QJsonDocument` | AppCore, PlatformConfiguration | Parse `.tcnf` configuration files and `devicelist.json` |
| `QJsonObject` | AppCore, PlatformConfiguration, USBDescriptors | Access individual fields in config JSON |
| `QJsonArray` | PlatformConfiguration | Iterate pin/button/command arrays in config |

**Threading & Synchronization (~6 files)**

| Qt Class | Where Used | Role |
|----------|-----------|------|
| `QThread` | DriveThread (inherits) | Background I/O thread for command execution |
| `QMutex` | AlpacaDevice, DriveThread | Protect shared device state |
| `QMutexLocker` | DriveThread | RAII locking |

**Signals & Slots (~10 classes)**

| Class with Q_OBJECT | Signals/Slots Used For |
|---------------------|----------------------|
| `SerialPort` | Data received, error occurred, port state changes |
| `DriveThread` | Command completed, progress updates |
| `AlpacaDevice` | Device state changes, connection events |
| `CommDevice` | Communication events |
| `FrameCoder` | Frame decoded/encoded notifications |
| `TACDevCore` | High-level device events forwarded to TACDev API |
| `DateCheckFailEvent` | Custom event for date validation |

**File System (~15 files)**

| Qt Class | Where Used | Role |
|----------|-----------|------|
| `QFile` | CommDevice, DriveThread, AppCore, FTDICheckApplication | Read config files, write logs |
| `QDir` | AppCore, main.cpp (various) | Locate config directory, enumerate files |
| `QFileInfo` | ConsoleApplicationEnhancements | Check file existence, get paths |

**Settings (~5 files)**

| Qt Class | Where Used | Role |
|----------|-----------|------|
| `QSettings` | AlpacaSettings, CommDevice, ConsoleApplicationEnhancements | Store/retrieve user preferences and device calibration |

**Other**

| Qt Class | Where Used | Role |
|----------|-----------|------|
| `QVariant` | AlpacaScript, ErrorParameter, FrameCoder, PlatformConfiguration | Generic value containers for script variables |
| `QSharedPointer<T>` | AlpacaDevice, AlpacaScript, FramePackage | Reference-counted ownership |
| `QDate` / `QDateTime` | AppCore, AlpacaSharedLibrary, CommDevice, DriveThread | Timestamps and date validation |
| `QSize` / `QPoint` / `QPointF` | PlatformConfiguration, AlpacaDevice, DataPoints | UI layout hints stored in config (can be simple structs) |
| `QProcess` | ConsoleApplicationEnhancements | Subprocess execution for driver install |
| `QLibrary` | ConsoleApplicationEnhancements | Dynamic library loading |
| `QBitArray` | CharBit | Bit-level manipulation |
| `QTextStream` | AlpacaScript, AlpacaSharedLibrary, CSV | Text parsing |
| `QEvent` | DateCheckFailEvent | Custom event type |

### 2.3 Qt Classes Used in TACDev Interface

| Qt Class | Where Used | Role |
|----------|-----------|------|
| `QString` | TACDevCore.h/cpp | String parameters passed to/from QCommonConsole |
| `QMap` | TACDevCore.h | Stores device handle-to-object mapping |
| `QObject` / `Q_OBJECT` | TACDevCore | Receives signals from core device objects |
| `QSerialPort` (indirect) | Via QCommonConsole linkage | Inherited dependency |

---

## 3. Replacement Strategy

### 3.1 Standard C++ Replacements (no external dependency)

| Qt Class | Replacement | Standard | Notes |
|----------|-------------|----------|-------|
| `QString` | `std::string` | C++17 | UTF-8 by convention; `std::wstring` only at Win32 API boundary |
| `QByteArray` | `std::vector<uint8_t>` | C++17 | For serial I/O buffers. Use `std::string` where text semantics apply |
| `QStringList` | `std::vector<std::string>` | C++17 | |
| `QList<T>` / `QVector<T>` | `std::vector<T>` | C++17 | |
| `QMap<K,V>` | `std::map<K,V>` | C++17 | Use `std::unordered_map` where ordering is irrelevant |
| `QPair<T,U>` | `std::pair<T,U>` | C++17 | |
| `QSharedPointer<T>` | `std::shared_ptr<T>` | C++17 | |
| `QVariant` | `std::variant<types...>` or `std::any` | C++17 | Define a `ScriptValue` variant with the actual used types |
| `QFile` / `QDir` / `QFileInfo` | `std::filesystem` + `std::fstream` | C++17 | |
| `QTextStream` | `std::istringstream` / `std::ostringstream` | C++17 | |
| `QThread` | `std::jthread` | C++20 | Cooperative cancellation via `std::stop_token` |
| `QMutex` / `QMutexLocker` | `std::mutex` / `std::lock_guard` | C++17 | |
| `QDate` / `QDateTime` | `std::chrono::system_clock` / `std::chrono::year_month_day` | C++20 | |
| `QSize` / `QPoint` / `QPointF` | `struct Size { int w, h; }` etc. | Custom | Trivial POD structs |
| `QBitArray` | `std::bitset<N>` or `std::vector<bool>` | C++17 | |
| `Q_NULLPTR` | `nullptr` | C++11 | |
| `Q_ASSERT` | `assert()` or custom `QTAC_ASSERT` | C | |
| `Q_OS_WINDOWS` / `Q_OS_LINUX` | `#ifdef _WIN32` / `#ifdef __linux__` | Preprocessor | |
| `QEvent` | Plain virtual class | Custom | No Qt event loop needed |

### 3.2 External Libraries (minimal, vendorable)

Only two external dependencies are introduced, both header-only and MIT-licensed:

| Replaces | Library | Type | License | Size |
|----------|---------|------|---------|------|
| `QJsonDocument`, `QJsonObject`, `QJsonArray` | [nlohmann/json](https://github.com/nlohmann/json) | Header-only | MIT | Single .hpp file (~800 KB) |
| `QSettings` (INI read/write) | [inih](https://github.com/benhoyt/inih) | Single C file + header | BSD | ~400 lines |

Both are vendored directly into the repository — no package manager or install step required.

### 3.3 Custom Platform Abstractions

These replace Qt modules that wrap OS-specific APIs. Each is a single header with two implementation files (Win32 + POSIX):

| Replaces | New Abstraction | Estimated Size | Notes |
|----------|----------------|----------------|-------|
| `QSerialPort` / `QSerialPortInfo` | `qtac::SerialPort` | ~300 lines per platform | Open, close, configure, read, write, enumerate. Pimpl pattern. |
| `QProcess` | `qtac::Process` | ~150 lines per platform | `CreateProcess` (Win) / `fork+exec` (POSIX). Used only in ConsoleApplicationEnhancements. |
| `QLibrary` | `qtac::DynamicLibrary` | ~50 lines per platform | `LoadLibrary`/`GetProcAddress` (Win) / `dlopen`/`dlsym` (POSIX) |
| Signals/Slots | `qtac::Signal<Args...>` | ~40 lines (portable) | `std::function` + `std::vector` + `std::mutex`. See Section 4.1. |

---

## 4. Key Design Patterns

### 4.1 Signal/Slot Replacement

Qt's meta-object signal/slot system is used by ~10 core classes. Replace with a lightweight, thread-safe callback mechanism:

```cpp
// qtac/signal.h
#pragma once
#include <functional>
#include <vector>
#include <mutex>
#include <algorithm>

namespace qtac {

template <typename... Args>
class Signal {
public:
    using Slot = std::function<void(Args...)>;

    int connect(Slot slot) {
        std::lock_guard lock(mutex_);
        int id = next_id_++;
        slots_.push_back({id, std::move(slot)});
        return id;
    }

    void disconnect(int id) {
        std::lock_guard lock(mutex_);
        std::erase_if(slots_, [id](const auto& s) { return s.id == id; });
    }

    void operator()(Args... args) {
        std::lock_guard lock(mutex_);
        for (auto& s : slots_) s.fn(args...);
    }

private:
    struct Entry { int id; Slot fn; };
    std::vector<Entry> slots_;
    std::mutex mutex_;
    int next_id_ = 0;
};

} // namespace qtac
```

**Before (Qt):**
```cpp
class DriveThread : public QThread {
    Q_OBJECT
signals:
    void commandCompleted(const QString& cmd, bool success);
public slots:
    void onNewCommand(const QString& cmd);
};

// caller
QObject::connect(device, &AlpacaDevice::sendCommand,
                 driveThread, &DriveThread::onNewCommand);
```

**After (Standard C++):**
```cpp
class DriveThread {
public:
    qtac::Signal<const std::string&, bool> commandCompleted;
    void onNewCommand(const std::string& cmd);
    void start();
    void stop();
private:
    std::jthread thread_;
};

// caller
device->sendCommand.connect([&](const std::string& cmd) {
    driveThread->onNewCommand(cmd);
});
```

### 4.2 SerialPort Abstraction

```cpp
// qtac/platform/serial_port.h
#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace qtac {

enum class Parity   { None, Even, Odd };
enum class StopBits { One, Two };
enum class FlowCtrl { None, Hardware, Software };

struct SerialPortInfo {
    std::string port_name;      // "COM3" or "/dev/ttyUSB0"
    std::string description;
    std::string serial_number;
    uint16_t    vid = 0;
    uint16_t    pid = 0;
};

class SerialPort {
public:
    SerialPort();
    ~SerialPort();
    SerialPort(SerialPort&&) noexcept;
    SerialPort& operator=(SerialPort&&) noexcept;

    bool open(const std::string& port_name);
    void close();
    bool is_open() const;

    bool set_baud_rate(uint32_t baud);
    bool set_data_bits(uint8_t bits);
    bool set_parity(Parity p);
    bool set_stop_bits(StopBits sb);
    bool set_flow_control(FlowCtrl fc);

    int64_t read(uint8_t* buf, size_t max_bytes, uint32_t timeout_ms = 1000);
    int64_t write(const uint8_t* buf, size_t len);

    static std::vector<SerialPortInfo> enumerate();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace qtac
```

Platform backends:
- `serial_port_win32.cpp` — `CreateFile`, `SetCommState`, `SetCommTimeouts`, `ReadFile`, `WriteFile`, `SetupComm`
- `serial_port_posix.cpp` — `open`, `tcsetattr`, `cfsetispeed`, `read`, `write`, `select`

### 4.3 DriveThread Without QThread

```cpp
class DriveThread {
public:
    void start() {
        thread_ = std::jthread([this](std::stop_token st) { run(st); });
    }

    void stop() {
        thread_.request_stop();
        cv_.notify_all();
    }

    void enqueue(TACCommand cmd) {
        {
            std::lock_guard lock(mutex_);
            queue_.push(std::move(cmd));
        }
        cv_.notify_one();
    }

    qtac::Signal<const std::string&, bool> commandCompleted;

private:
    void run(std::stop_token st) {
        while (!st.stop_requested()) {
            std::unique_lock lock(mutex_);
            cv_.wait(lock, st, [&] { return !queue_.empty(); });
            if (st.stop_requested()) break;
            auto cmd = std::move(queue_.front());
            queue_.pop();
            lock.unlock();
            execute(cmd);
        }
    }

    void execute(const TACCommand& cmd);

    std::jthread thread_;
    std::mutex mutex_;
    std::condition_variable_any cv_;
    std::queue<TACCommand> queue_;
};
```

### 4.4 JSON / Configuration Loading

**Before (Qt):**
```cpp
QFile file(configPath);
file.open(QIODevice::ReadOnly);
QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
QJsonObject root = doc.object();
QString name = root["name"].toString();
QJsonArray pins = root["pins"].toArray();
```

**After (nlohmann/json):**
```cpp
std::ifstream file(config_path);
auto root = nlohmann::json::parse(file);
std::string name = root["name"].get<std::string>();
auto& pins = root["pins"];  // iterate directly
```

---

## 5. Directory Structure After Migration

```
src/libraries/qcommon-console/
├── include/qtac/                        # Public headers
│   ├── alpaca_device.h
│   ├── ftdi_device.h
│   ├── psoc_device.h
│   ├── pic32cx_device.h
│   ├── platform_configuration.h
│   ├── drive_thread.h
│   ├── tac_command.h
│   ├── alpaca_script.h
│   ├── usb_descriptors.h
│   ├── signal.h                         # Signal<> utility
│   └── platform/                        # Platform abstractions
│       ├── serial_port.h
│       ├── process.h
│       └── dynamic_library.h
├── src/
│   ├── alpaca_device.cpp
│   ├── drive_thread.cpp
│   ├── platform_configuration.cpp
│   ├── tac_command.cpp
│   ├── alpaca_script.cpp
│   ├── ftdi_device.cpp
│   ├── psoc_device.cpp
│   ├── pic32cx_device.cpp
│   └── platform/
│       ├── serial_port_win32.cpp
│       ├── serial_port_posix.cpp
│       ├── process_win32.cpp
│       ├── process_posix.cpp
│       ├── dynamic_library_win32.cpp
│       └── dynamic_library_posix.cpp
├── third-party/
│   ├── nlohmann/json.hpp                # Vendored, MIT
│   └── inih/ini.h + ini.c              # Vendored, BSD
└── CMakeLists.txt                       # No find_package(Qt6)

interfaces/C++/TACDev/
├── include/
│   └── tacdev.h                         # Pure C API (no Qt types)
├── src/
│   └── tacdev_core.cpp                  # Links QCommonConsole only
└── CMakeLists.txt                       # No find_package(Qt6)
```

---

## 6. Impact on Consumers

### 6.1 GUI Applications (QTAC, Device Catalog, Config Editor)

GUI apps retain Qt for their own UI layer. They consume the now-pure-C++ core library directly. A thin adapter utility handles type conversion at the GUI boundary:

```
src/libraries/qt-adapter/
    include/
        qt_string_convert.h      # std::string <-> QString
        qt_signal_bridge.h       # qtac::Signal -> QObject signal forwarding
    src/
        qt_signal_bridge.cpp
```

```cpp
// qt_string_convert.h
#pragma once
#include <QString>
#include <string>

namespace qtac::qt {
inline QString toQt(const std::string& s) { return QString::fromStdString(s); }
inline std::string fromQt(const QString& s) { return s.toStdString(); }
inline QStringList toQt(const std::vector<std::string>& v) {
    QStringList out;
    for (auto& s : v) out.append(QString::fromStdString(s));
    return out;
}
}
```

**GUI applications are not modified in this proposal** — they only gain a new `qt-adapter` library to link against.

### 6.2 CLI Applications

CLI tools (`devlist`, `tacdump`, `ftdi-check`, etc.) will automatically lose their transitive Qt dependency once the core libraries are migrated. Their own source code may need minor updates to use `std::string` instead of `QString` for values received from the core library, but those changes are mechanical and small.

### 6.3 Language Bindings (Python, C#, Java)

The TACDev C API (`tacdev.h`) already exports `const char*` strings, `int` handles, and `bool` states. The public API surface does not change — bindings continue to work without modification.

### 6.4 Automation Scripts (Python TACDev)

No change. The Python wrapper calls the C API via ctypes. The underlying library implementation is invisible.

---

## 7. Migration Plan

### Phase 0: Scaffolding (1 week)

| Task | Details |
|------|---------|
| Create feature branch `feature/remove-qt-core` | All work happens here |
| Introduce `namespace qtac` | New/migrated headers use `qtac::` namespace |
| Vendor `nlohmann/json.hpp` into `third-party/nlohmann/` | Single file drop |
| Vendor `inih` into `third-party/inih/` | Two files (ini.h + ini.c) |
| Write `qtac::Signal<>` header | ~40 lines, see Section 4.1 |
| Write platform abstraction headers | `serial_port.h`, `process.h`, `dynamic_library.h` — interfaces only |
| Set up dual-build CMake option | `option(QTAC_USE_QT "Build with Qt support" ON)` — allows gradual migration |

### Phase 1: Type Replacements (2-3 weeks)

Mechanical, file-by-file replacement of Qt types with standard equivalents. Low risk — each replacement is a direct 1:1 swap.

| Step | Replacement | Files | Risk |
|------|------------|-------|------|
| 1a | `QString` -> `std::string` | ~70 | Low — API-visible, but mostly internal |
| 1b | `QByteArray` -> `std::vector<uint8_t>` | ~50 | Low — check serial read/write call sites |
| 1c | `QList`/`QVector` -> `std::vector` | ~30 | Low — near-identical API |
| 1d | `QMap` -> `std::map` | ~15 | Low — iterator differences are minor |
| 1e | `QSharedPointer` -> `std::shared_ptr` | ~8 | Low — same semantics |
| 1f | `QPair` -> `std::pair` | ~3 | Low |
| 1g | `QVariant` -> `std::variant` / `std::any` | ~5 | Medium — need to define the variant type set |
| 1h | `QDate`/`QDateTime` -> `std::chrono` | ~4 | Low |
| 1i | `QFile`/`QDir`/`QFileInfo` -> `std::filesystem` + `std::fstream` | ~15 | Low |
| 1j | `QTextStream` -> `std::stringstream` | ~4 | Low |
| 1k | `QBitArray` -> `std::bitset` | ~1 | Low |
| 1l | `Q_NULLPTR`/`Q_ASSERT`/`Q_OS_*` macros | scattered | Low |

### Phase 2: Platform Abstractions (2-3 weeks)

Replace Qt modules that wrap OS-specific functionality. **Highest-risk phase** — serial port correctness is critical.

| Step | Component | Approach | Risk |
|------|-----------|----------|------|
| 2a | `qtac::SerialPort` | Win32 + POSIX implementations (~300 lines each) | **High** — must match QSerialPort behavior exactly |
| 2b | `qtac::Process` | `CreateProcess` / `fork+exec` | Medium — used in limited places |
| 2c | `qtac::DynamicLibrary` | `LoadLibrary` / `dlopen` | Low — simple wrapper |
| 2d | INI settings | Replace `QSettings` with `inih` | Low |

**Mitigation for 2a:** Write a serial port test harness that runs the same operations through both `QSerialPort` and `qtac::SerialPort`, comparing results on real hardware. Keep `QSerialPort` available behind the CMake flag during transition.

### Phase 3: Signal/Slot & Threading (1-2 weeks)

| Step | Component | Approach | Risk |
|------|-----------|----------|------|
| 3a | Remove `Q_OBJECT` / MOC | Replace all `signals:` with `qtac::Signal<>` members | Medium — must review every `connect()` call |
| 3b | `QThread` -> `std::jthread` | Rewrite `DriveThread` (see Section 4.3) | Medium — thread lifecycle must be tested |
| 3c | `QMutex` -> `std::mutex` | Direct replacement | Low |
| 3d | `QEvent` subclass | Replace with plain virtual class | Low |

### Phase 4: JSON & Configuration (1 week)

| Step | Component | Approach | Risk |
|------|-----------|----------|------|
| 4a | `.tcnf` file loading | Rewrite `PlatformConfiguration` parser using `nlohmann::json` | Medium — must validate all config files parse identically |
| 4b | `devicelist.json` loading | Rewrite `AppCore` / `USBDescriptors` JSON code | Low |

**Validation:** Write a test that loads every `.tcnf` file and `devicelist.json` with both the old Qt parser and the new nlohmann parser, diffing the resulting in-memory structures.

### Phase 5: Cleanup & Validation (1-2 weeks)

| Task | Details |
|------|---------|
| Remove `find_package(Qt6)` from QCommonConsole and TACDev CMakeLists | The core libraries no longer depend on Qt |
| Remove Qt includes from all core headers | Verify no transitive Qt leakage |
| Build QCommonConsole + TACDev without Qt installed | The litmus test |
| Verify GUI apps still compile and run | They link `qt-adapter` + Qt for their own UI |
| Cross-platform CI | Build and test on Windows (MSVC 2022), Linux (GCC 11+), optionally macOS (Clang) |
| Test with real hardware | Exercise all three board types (FTDI, PSoC, PIC32CX) on both platforms |

---

## 8. Risk Assessment

| Risk | Likelihood | Impact | Mitigation |
|------|-----------|--------|------------|
| Serial port behavior regression | Medium | **Critical** | Side-by-side test harness comparing `QSerialPort` vs `qtac::SerialPort` on real hardware; keep Qt fallback via CMake flag during transition |
| String encoding issues (UTF-8 vs UTF-16) | Medium | Medium | Standardize on UTF-8 in all core code; convert only at Win32 API boundaries |
| Signal/slot race conditions | Low | High | `qtac::Signal<>` is mutex-protected; review every cross-thread signal path |
| JSON parsing differences | Low | Medium | Automated test: parse all `.tcnf` files with both parsers, compare output |
| GUI regression | Low | Medium | GUI adapter isolates core changes from UI layer; minimal GUI code changes |
| Build breakage during migration | High | Low | Feature branch; phase-by-phase PRs; CI gates on both platforms |
| `std::filesystem` inconsistencies | Low | Low | Well-supported since GCC 8+, MSVC 2017+, Clang 7+ |

---

## 9. Benefits

| Benefit | Details |
|---------|---------|
| **Zero-dependency core** | QCommonConsole and TACDev are self-contained — no Qt runtime needed |
| **~90% smaller binaries** | Consumers (CLI tools, scripts) go from ~60 MB to ~2-5 MB |
| **Any-compiler builds** | Core library compiles with any C++20 toolchain, no Qt SDK |
| **Faster builds** | No MOC step, no Qt header parsing overhead |
| **Easier cross-compilation** | Pure C++ cross-compiles trivially for ARM, RISC-V, embedded targets |
| **Simpler onboarding** | Contributors need only a compiler, not the full Qt development environment |
| **No licensing concerns** | Only MIT/BSD vendored dependencies — no LGPL propagation |
| **Cleaner architecture** | GUI concerns are fully separated from core device logic |

---

## 10. Estimated Effort

| Phase | Duration | Developers |
|-------|----------|------------|
| Phase 0: Scaffolding | 1 week | 1 |
| Phase 1: Type replacements | 2-3 weeks | 1-2 |
| Phase 2: Platform abstractions | 2-3 weeks | 1-2 |
| Phase 3: Signals & threading | 1-2 weeks | 1 |
| Phase 4: JSON & config | 1 week | 1 |
| Phase 5: Cleanup & validation | 1-2 weeks | 1-2 |
| **Total** | **8-12 weeks** | **~10-16 person-weeks** |

---

## 11. Open Decision Points

| # | Question | Options |
|---|----------|---------|
| 1 | **Serial port implementation** | (a) Custom Win32/POSIX abstraction — zero deps, full control, more code to maintain<br>(b) `libserialport` — proven, ~5 source files, LGPL license |
| 2 | **JSON library** | (a) `nlohmann/json` — ergonomic, header-only, MIT, widely used<br>(b) `RapidJSON` — faster parsing, MIT, more verbose API<br>(c) Custom minimal parser — zero deps, but significant work |
| 3 | **C++ standard level** | (a) C++17 — wider compiler support, use `std::thread` instead of `std::jthread`<br>(b) C++20 — `std::jthread`, `std::format`, calendar types, ranges |
| 4 | **Migration strategy** | (a) Single feature branch, phase-by-phase PRs merged into it, final merge to `develop`<br>(b) Incremental PRs directly to `develop` with `QTAC_USE_QT` CMake flag |
| 5 | **Library rename** | Keep `QCommonConsole` name (established) vs rename to `TACCore` or `qtac-core` (drop the Q) |
