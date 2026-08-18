# QTAC Qt Removal Refactor — Presentation
### Outline & Speaker Notes

---

## Slide 1 — Title

**Title:** Removing Qt from the QTAC Core Library
**Subtitle:** Architecture, Design Decisions, and Demo

**Speaker notes:**
This presentation covers the work done to remove Qt as a dependency from the QTAC device-control library — the code that talks to FTDI, PSoC, and PIC32CX debug boards. We'll walk through why it was worth doing, how we designed the solution, the key decisions we made along the way, and finish with a live demo comparing the new Qt-free app to the legacy TAC.exe.

---

## Slide 2 — What is QTAC / TAC?

**Content:**
- TAC = Test Automation Controller
- A tool used by hardware test engineers to control debug boards on DUT (Device Under Test)
- Boards supported: **FTDI-based ALPACA-LITE**, **PSoC**, **PIC32CX**
- Exposes pin control, serial communication, scripted quick-commands, device configuration
- Deliverables: `TAC.exe` (legacy GUI), `TACDev.dll` (C API for lab automation)

**Speaker notes:**
Most people in this room work with TAC at some point in bring-up or test automation. The tool lets you toggle GPIO pins, send serial commands, and run scripted sequences against debug boards. It's delivered both as a GUI (TAC.exe) and as a DLL that test scripts can call programmatically. Understanding that dual-delivery model is important for why the Qt dependency was a problem.

---

## Slide 3 — The Problem: Qt Entangled in the Core

**Content:**
**Before — Qt entangled in every layer:**

```
┌─────────────────────────────────────────────────────────────┐
│                  QCommonConsole (Qt-dependent)               │
│                                                             │
│   ┌──────────────┐  ┌──────────────┐  ┌──────────────┐     │
│   │  Device      │  │  Protocols   │  │  Serial      │     │
│   │  Logic       │  │  TACLite     │  │  QSerialPort │     │
│   │  AlpacaDevice│  │  PSOC        │  │              │     │
│   └──────┬───────┘  └──────┬───────┘  └──────┬───────┘     │
│          │                 │                  │             │
│          └─────────────────┴──────────────────┘             │
│                            │                                │
│                    ┌───────▼────────┐                       │
│                    │   Qt6::Core    │  ← required by ALL    │
│                    │   QString      │                       │
│                    │   QByteArray   │                       │
│                    │   QObject/moc  │                       │
│                    │   QSerialPort  │                       │
│                    └───────┬────────┘                       │
└───────────────────────────┬─────────────────────────────────┘
                            │  Qt6 required by every consumer
            ┌───────────────┼───────────────┐
            ▼               ▼               ▼
       TAC.exe (GUI)   TACDev.dll (C API)  CLI tools
                       ← lab scripts use this
                         but must install Qt6 (~600 MB)
```

**After — Qt isolated to the GUI layer only:**

```
                    ┌─────────────────────────┐
                    │    qtac-app.exe (GUI)    │  Qt6 here only
                    │    TACWindow, TACPinFrame│
                    └────────────┬────────────┘
                                 │
                    ┌────────────▼────────────┐
                    │  qt-adapter (bridge)    │  Qt6::Core only
                    │  TACDeviceBridge        │
                    └────────────┬────────────┘
                                 │ C++20 only below this line
          ┌──────────────────────▼──────────────────────┐
          │              qtac-core (static lib)          │
          │  AlpacaDevice  TACDriveThread  AlpacaScript  │
          │  qtac::String  qtac::Signal<>  SerialPort     │
          │              zero Qt dependency               │
          └──────────────┬───────────────────────────────┘
                         │
                ┌────────▼────────┐
                │  TACDev.dll     │  no Qt, no Qt runtime
                │  C API          │  ~300 KB
                └─────────────────┘
```

Legacy `QCommonConsole` library:
- Device logic, hardware protocols, and GUI all lived in **one Qt-dependent library**
- `TACDev.dll` was built from the same Qt-linked code
- Every consumer — CLI, GUI, DLL — **required Qt6 to be installed**
- Qt6 is a ~600 MB runtime; deploying it for a headless DLL is wasteful and fragile
- Qt's licensing (LGPL) **prohibits redistribution** — a blocker for open-sourcing QTAC
- An outside developer could not build or contribute to the project without a full Qt6 install

**Speaker notes:**
The core problem was architectural: Qt had crept into every layer. Even TACDev.dll, which is a headless C API used in Python test scripts, had to carry Qt6 as a runtime dependency. That's a 600 MB framework dragged in just to flip a GPIO pin. But the biggest driver was open-source: we are releasing QTAC publicly, and Qt's LGPL license prohibits distributing Qt itself alongside the project. That meant any external developer wanting to build or contribute had to separately acquire and install Qt6 — a significant barrier to entry. The goal was to cut Qt out of the device core entirely so the library can be built, used, and contributed to with no Qt dependency at all.

---

## Slide 4 — Goal and Scope

**Content:**

| Layer | Qt allowed? | Scope |
|-------|-------------|-------|
| `source/library` (qtac-core) | **No** | ✅ Refactored |
| `source/tacdev` (TACDev.dll) | **No** | ✅ Refactored — Qt-free |
| `source/libraries/qt-adapter` | Yes (bridge) | ✅ New bridge |
| `source/app` (qtac-app.exe) | Yes | ✅ New Qt6 GUI using clean core |

**Speaker notes:**
The refactor drew a clear line. Everything under `source/` is new: the Qt-free library, a refactored TACDev.dll, a thin Qt bridge, and a new Qt6 GUI that uses *only* the clean core. That boundary discipline was critical to avoiding scope creep.

---

## Slide 5 — Three Approaches Considered

**Content:**

| Approach | Call-site changes | Risk | Readability |
|----------|-------------------|------|-------------|
| **A. `qtac::` wrappers** *(chosen)* | Minimal — type names only | Low | High — Qt-like |
| B. Direct `std::` replacement | Extensive — every method call | Medium | Medium |
| C. Centralized `StringUtils` functions | Extensive — every call site | Medium | Medium |

**Speaker notes:**
Early in planning we studied a parallel `tac-core` project that went the direct `std::` route. That approach is principled but creates enormous diffs — every `str.contains()` becomes `str.find() != std::string::npos`, every `str.split(',')` becomes 8 lines of `istringstream`. We counted 159 such call sites in the codebase. With the wrapper approach, the diff for migrating a file is mostly just type-name substitutions. That reduces review burden, lowers the chance of introducing bugs, and lets engineers who know Qt read the code immediately.

---

## Slide 6 — Approach A: `qtac::` Wrappers (Chosen)

**Content:**

*Before (Qt):*
```cpp
QString processInput(const QString& input) {
    QStringList parts = input.trimmed().toLower().split(',');
    return parts.join(" | ");
}
```

*After (`qtac::` wrappers):*
```cpp
qtac::String processInput(const qtac::String& input) {
    qtac::StringList parts = input.trimmed().toLower().split(',');
    return parts.join(" | ");
}
```

*With direct `std::` (Approach B):*
```cpp
std::string processInput(const std::string& input) {
    // 20+ lines: manual trim, transform, istringstream, join loop
}
```

**Speaker notes:**
This slide makes the tradeoff concrete. The `qtac::` version is a two-word change: replace `QString` with `qtac::String`, replace `QStringList` with `qtac::StringList`. Logic and method names are identical. The direct `std::` version turns two readable lines into twenty lines of manual implementation — and that pattern repeated itself 159 times across the codebase. The wrapper approach cost us ~2000 lines of infrastructure code up front, but saved thousands of call-site rewrites and dramatically reduced risk.

---

## Slide 7 — Key Qt Replacement Types

**Content:**

| Qt Type | `qtac::` Equivalent | Backed by |
|---------|--------------------|-----------|
| `QString` | `qtac::String` | `std::string` |
| `QByteArray` | `qtac::ByteArray` | `std::string` |
| `QStringList` | `qtac::StringList` | `std::vector<qtac::String>` |
| `QList<T>` | `qtac::List<T>` | `std::vector<T>` |
| `QMap<K,V>` | `qtac::Map<K,V>` | `std::map<K,V>` |
| `QVariant` | `qtac::Variant` | `std::variant` |
| `QSharedPointer<T>` | `std::shared_ptr<T>` | C++11 stdlib |
| `QMutex` / `QWaitCondition` | `std::mutex` / `std::condition_variable` | C++11 stdlib |
| `QPair<A,B>` | `qtac::Pair<A,B>` | `std::pair<A,B>` |
| `QSerialPort` / `QSerialPortInfo` | `qtac::SerialPort` / `qtac::SerialPortInfo` | libserialport |

**Speaker notes:**
Every Qt type used in the device core has a direct `qtac::` equivalent. The wrappers expose the same method names: `toLower()`, `contains()`, `split()`, `join()`, `append()`, `isEmpty()` — so migrated code reads exactly like Qt code. Internally, everything is backed by standard library types, so there's no Qt runtime dependency. `QSerialPort` was the highest-risk substitution because it crosses into OS APIs; we solved that with `libserialport`, a vendored cross-platform serial library.

---

## Slide 8 — The Biggest Design Challenge: Signals & Slots

**Content:**

*Legacy Qt signals (in `TACDriveThread`):*
```cpp
// Q_OBJECT in class, moc generates glue code
signals:
    void pinStateChanged(quint64 pin, bool state);
    void deviceOpen();
    void firmwareVersionUpdate(QString firmware);
    // ... 14 signals total
```

*Why Qt signals are hard to replace:*
- **Multi-subscriber**: many listeners can connect to one signal
- **Thread-safe dispatch**: automatically marshals to receiver's thread
- **Auto-disconnect**: connection breaks when object is destroyed

**Speaker notes:**
Qt's signal/slot system does three things at once that pure `std::function` callbacks don't: multiple subscribers, thread-safe delivery, and automatic lifetime cleanup. An earlier parallel `tac-core` fork replaced signals with bare `std::function` members — but a `std::function` only holds one callback. Plug in a second one and you silently overwrite the first. And there's no thread safety: callbacks fired from the device thread would run on the wrong thread and corrupt GUI state.

---

## Slide 9 — Solution: `qtac::Signal<Args...>`

**Content:**

```cpp
// qtac::Signal — multi-subscriber, lifetime-safe, Qt-free
template<typename... Args>
class Signal {
public:
    // Connect: returns a token; hold a shared_ptr to stay connected
    std::shared_ptr<void> connect(std::function<void(Args...)> fn);

    // Fire: calls all live subscribers
    void operator()(Args... args);
};
```

*Usage — identical feel to Qt signals:*
```cpp
// In TACDriveThread:
qtac::Signal<uint64_t, bool>        onPinStateChanged;
qtac::Signal<const qtac::String&>   onFirmwareVersionUpdate;

// In calling code:
_token = thread->onPinStateChanged.connect([this](uint64_t pin, bool state) {
    // handle pin change
});
// token goes out of scope → auto-disconnect
```

**Speaker notes:**
`qtac::Signal` is a small template we wrote to replace Qt signals without Qt. It uses a list of `weak_ptr`-wrapped callbacks so that when a subscriber is destroyed, its entry is automatically cleaned up on the next fire — no dangling pointers, no explicit disconnect required. Multiple subscribers are supported. Thread marshaling to the GUI thread is handled separately by the Qt adapter bridge, which we'll cover next. This was a deliberate separation of concerns: the core library handles *what* happened; the bridge handles *where* to deliver it.

---

## Slide 10 — The Qt Bridge: Clean Separation

**Content:**

```
┌──────────────────────────────────────────┐
│          source/app  (Qt6 GUI)           │  ← uses Qt freely
│  TACWindow, TACPinFrame, TACDeviceSelection │
└─────────────────┬────────────────────────┘
                  │ connects Qt signals to GUI widgets
┌─────────────────▼────────────────────────┐
│   source/libraries/qt-adapter            │  ← thin bridge
│   TACDeviceBridge  (Q_OBJECT)            │
│   • Wraps qtac::TACDriveThread           │
│   • Receives qtac::Signal callbacks      │
│   • Re-emits as Qt signals               │
│     via QMetaObject::invokeMethod        │
│     (thread-safe queue crossing)         │
└─────────────────┬────────────────────────┘
                  │ uses only C++20
┌─────────────────▼────────────────────────┐
│   source/library  (qtac-core)            │  ← zero Qt
│   AlpacaDevice, TACDriveThread, ...      │
└──────────────────────────────────────────┘
```

**Speaker notes:**
The bridge pattern is what makes this architecture clean. `TACDeviceBridge` is the only class that's allowed to know about both worlds. It connects to `qtac::Signal<>` callbacks from the device thread, and when they fire — on the device thread — it uses `QMetaObject::invokeMethod` with `Qt::QueuedConnection` to safely marshal the event onto the Qt GUI thread. The GUI code above the bridge doesn't know or care that the device library is Qt-free. And the device library below the bridge doesn't know or care about Qt. The bridge is the seam.

---

## Slide 11 — TACDev.dll: Refactored to Qt-Free

**Content:**

```c
// TACDev.h — pure C API, no Qt, no C++ exceptions crossing DLL boundary
TACDEV_API TACDevHandle  TACDev_Open(const char* portName);
TACDEV_API void          TACDev_Close(TACDevHandle h);
TACDEV_API int           TACDev_SendCommand(TACDevHandle h, const char* cmd, bool on);
TACDEV_API int           TACDev_SetPinState(TACDevHandle h, uint64_t pin, bool state);
TACDEV_API int           TACDev_QuickCommand(TACDevHandle h, const char* name);
TACDEV_API const char*   TACDev_GetFirmwareVersion(TACDevHandle h);
// ... and more
```

*Before:*
- TACDev.dll linked Qt6Core.dll (~8 MB) + Qt6SerialPort.dll + ICU (~30 MB)
- Required Qt6 runtime on every lab machine

*After:*
- TACDev.dll links only `ftd2xx.dll` (FTDI) or has no external dependencies for serial devices
- Total DLL footprint: **~300 KB**

**Speaker notes:**
TACDev.dll has always been how Python test scripts on lab machines interact with TAC devices — open a device, toggle a pin, send a command. The legacy DLL dragged in Qt6 for that: ~38 MB of runtime that had to be installed on every lab machine. The refactored TACDev.dll is built purely from `qtac-core`, exposes the same C API, and has no Qt dependency at all. Lab machine setup goes from "install Qt6 runtime" to "drop the DLL next to the script." The C API boundary also means any language — Python, C#, LabVIEW — can use it without C++ ABI concerns.

---

## Slide 12 — Implementation: Waves / Phases

**Content:**

| Wave | Work |
|------|------|
| W1 | Platform configs, PSoC/PIC32CX device classes, devicelist.json loader |
| W2 | `qtac::SerialPort/SerialPortInfo` (libserialport); Qt-free drive thread hierarchy |
| W3 | `qtac::Signal<>` — multi-subscriber, lifetime-safe; migrate all callbacks |
| W4 | `qt-adapter` library — string/container converters + `TACDeviceBridge` |
| W5–6 | Wire `FTDIDevice::open()` to `TACDriveThread`; fix FTDI hardware bugs; VEH crash logger |
| W7 | Device Info tab, Terminal tab in new GUI |
| W8 | `qtac::AlpacaScript` engine; Quick Settings buttons + Variables UI |
| W9 | Preferences dialog, auto-shutdown, open-last-device |
| W10 | `TACDev.dll` — Qt-free C API wrapper |
| W11 | PSoC GUI path; PSoC command hash fix; decouple `source/` from `src/` |
| Fixes | PSoC platform ID fallback; `getPins()` override; TC_READY_N init; battery state; serial number |

**Speaker notes:**
The work was structured as progressive waves, each delivering a working, buildable checkpoint. We didn't attempt to land everything at once. Waves 1–4 built the foundation: types, serial, signals, bridge. Waves 5–9 built the GUI functionality. Wave 10 delivered TACDev.dll. Wave 11 cleaned up the build boundary between old and new code. Between the waves, a series of hardware parity fixes closed the gap between "compiles" and "works correctly with real devices."

---

## Slide 13 — Results: Before vs. After

**Content:**

| Metric | Before (legacy) | After (qtac-core + TACDev.dll) |
|--------|-----------------|-------------------------------|
| Qt dependency in device core | Yes (Qt6::Core, Qt6::SerialPort) | **None** |
| TACDev.dll runtime deps | Qt6Core.dll (~38 MB) | `ftd2xx.dll` only |
| TACDev.dll size | ~2 MB + ~38 MB Qt DLLs | **~300 KB** |
| Lab machine Qt install required | Yes | **No** |
| `QObject` / moc in core | Pervasive | **Zero** |
| Multi-subscriber callbacks | Via Qt signals | Via `qtac::Signal<>` |
| Thread-safe GUI delivery | Qt auto-marshaling | Via bridge + `QueuedConnection` |
| Test coverage | Manual only | Unit tests + hardware integration tests |
| PSoC / PIC32CX support | Legacy Qt path only | **New Qt-free path** |

**Speaker notes:**
The bottom line: TACDev.dll went from requiring a full Qt6 runtime to requiring only the FTDI DLL. The device core is genuinely Qt-free — zero `QObject` inheritance, zero `Q_OBJECT` macros, zero `moc` compilation steps. We added automated tests — both unit tests for coders/commands and hardware integration tests. And we added PSoC and PIC32CX support in the new path, which wasn't present before.

---

## Slide 14 — Demo: New GUI vs. Legacy TAC.exe

**[LIVE DEMO — suggested flow]**

### Legacy TAC.exe
1. Launch `TAC.exe`
2. Connect to ALPACA-LITE MTP8975 (platform 13)
3. Show device info: hardware type, firmware, serial, platform ID
4. Show pin tabs: General, FTDI pin layout, Terminal
5. Show Quick Settings buttons
6. Show Variables group
7. Toggle a pin — show LED/physical response

### New `qtac-app.exe`
1. Launch `qtac-app.exe`
2. Connect to same device (device selection dialog)
3. Show Device Info tab — same fields
4. Show pin tabs — General, tcnf-defined tabs, Terminal
5. Show Quick Settings buttons, Variables — same functionality
6. Toggle same pin — verify parity
7. Show Preferences → auto-shutdown, open last device
8. Disconnect and reconnect — show "open last device on startup"

**Speaker notes:**
The key things to highlight in the demo: the new GUI has feature parity with TAC.exe for the device types tested (FTDI, PSoC, PIC32CX). The Device Info tab surfaces the same metadata. Pin state responds correctly. Quick Settings buttons execute scripts. The Preferences dialog adds features the legacy app lacks (auto-shutdown, persistent last device). If time allows, see Appendix H for a TACDev.dll Python demo showing device control with no GUI and no Qt.

---

## Slide 15 — Known Gaps / Future Work

**Content:**

- Some legacy GUI features not yet ported (legacy help system, some device-type-specific dialogs)
- Linux support: `SerialPortInfo_posix.cpp` is stubbed — `libserialport` is cross-platform but Win32 path fully tested
- `AlpacaScript` validation (`validateScript()`) not yet exposed in new API
- Full automated test suite against all hardware variants still in progress

**Speaker notes:**
The refactor is functional and feature-complete for the primary use cases, but it's not a final replacement yet. The biggest remaining item is getting the new build into regular use so we can find and fix edge cases with real hardware. Linux support is architecturally solved (libserialport handles it) but not tested on actual hardware yet. These are all tractable follow-on items, not architectural risks.

---

## Slide 16 — Summary

**Content:**

> **We removed Qt from the QTAC device core without breaking any existing functionality.**

Key decisions:
1. **`qtac::` wrappers** — minimal call-site changes, Qt-like API, low migration risk
2. **`qtac::Signal<>`** — multi-subscriber, lifetime-safe, no Qt meta-object system
3. **Bridge pattern** — clean separation between core and Qt GUI
4. **Preserve legacy code** — zero risk to production; new code coexists
5. **TACDev.dll** — refactored to Qt-free C API, 100× smaller deployment footprint

**Speaker notes:**
The refactor delivered what it set out to do: a device library that is genuinely independent of Qt, deployable without a Qt runtime, and usable from any language via the C API. The architectural choices — wrappers, typed signals, bridge pattern, coexistence with legacy — were deliberate decisions to minimize risk and maximize code review efficiency. The foundation is now in place for the team to migrate off the legacy stack incrementally, at their own pace.

---

## Appendix A — File Map

```
source/
├── library/              ← qtac-core (Qt-free static library)
│   ├── include/qtac/     ← Public headers (qtac:: namespace)
│   └── src/              ← Implementations
├── libraries/
│   └── qt-adapter/       ← Bridge: qtac-core ↔ Qt (TACDeviceBridge, converters)
├── app/                  ← qtac-app.exe (Qt6 GUI using only qtac-core + adapter)
└── tacdev/               ← TACDev.dll (C API, Qt-free)

third-party/              ← nlohmann/json, libserialport (vendored)
docs/                     ← Planning documents, this presentation
```

---

## Appendix B — `qtac::Signal<>` Design

```
Signal<Args...>
├── connect(fn) → shared_ptr<token>
│     • wraps fn in a weak_ptr entry
│     • subscriber stays alive while token is held
│     • token destroyed → auto-disconnect on next fire
│
└── operator()(args...)
      • iterates subscriber list
      • promotes each weak_ptr; skips expired ones
      • fires all live subscribers
      • cleans up dead entries in-place
```

Thread safety: connect/disconnect and fire can happen concurrently via internal mutex. GUI thread marshaling is handled separately by `TACDeviceBridge::invokeMethod(Qt::QueuedConnection)`.

---

## Appendix C — Key Commits Reference

| Commit | Description |
|--------|-------------|
| W2: 88def5c | `qtac::SerialPort` + `qtac::SerialPortInfo` |
| W2: b4ebf76 | Qt-free drive thread hierarchy (TACLite, PSoC, PIC32CX) |
| W2: 490edaf | Migrate FramePackage + protocol layers to `qtac::ByteArray` |
| d70370e | `qtac::Signal<>` + migrate all callbacks |
| c560bb6 | `qt-adapter` library (TACDeviceBridge + converters) |
| f99befd | PSoC/PIC32CX platform configs + device classes |
| ee313d9 | `devicelist.json` loader + `.tcnf` config loader |
| 987b98e | W6: Fix FTDI connect for ALPACA-LITE MTP8975 |
| 13a95d3 | W8: `qtac::AlpacaScript` + Quick Settings UI |
| 501ba84 | W9: Preferences dialog + auto-shutdown |
| e1d0939 (W10) | Qt-free TACDev.dll |
| a5013c8 (W11) | Decouple `source/` from `src/` |
| 84880df | PSoC platform ID fallback (firmware_chip) |
| 05dff91 | Fix composite USB serial number (CM_Get_Parent) |
| 7b36a3e | Fix TC_READY_N disabled-pin initialization |

---

## Appendix D — AlpacaScript: Qt-Free Script Engine

**Content:**

*What AlpacaScript does:*
- Parses the `"script"` section of `.tcnf` device-config files
- Resolves named sub-functions (up to 7 levels of nesting)
- Substitutes `$VariableName` tokens at execution time
- Powers the "Quick Settings" buttons and user-configurable variables

*Legacy:*
```cpp
// In qcommon-console (Qt-dependent)
typedef QSharedPointer<_CommandEntry> CommandEntry;
typedef QList<CommandEntry>           CommandEntries;
QMap<QString, ScriptCommand>          _scriptCommands;
```

*Qt-free:*
```cpp
// In source/library/include/qtac/AlpacaScript.h
using CommandEntry   = std::shared_ptr<_CommandEntry>;
using CommandEntries = std::vector<CommandEntry>;
qtac::Map<qtac::String, ScriptCommand> _scriptCommands;
```

AlpacaScript is the mini-language embedded in device config files that drives the quick-command buttons. It was entirely Qt-dependent in the legacy library. The new version is a clean port: identical parsing logic, identical execution semantics, but using standard smart pointers and `qtac::` containers instead of Qt types. It also adds explicit `ButtonEntry` and `VariableEntry` structs that let the GUI enumerate available buttons and variables without knowing anything about the script internals.

---

## Appendix E — Platform Support: PSoC & PIC32CX

**Content:**

*Added alongside FTDI (ALPACA-LITE):*
- `TACPSOCDriveThread` — serial-based PSoC protocol over `qtac::SerialPort`
- `TACPIC32CXDriveThread` — serial-based PIC32CX protocol
- Platform identification: `devicelist.json` + `.tcnf` config files
- PSoC platform fallback chain:
  1. "Get Platform ID" command (hardware)
  2. USB bcdDevice revision → `PlatformContainer::fromRevision()`
  3. Firmware chip field → `PlatformContainer::fromFirmwareChip()`

*Key fix: composite USB serial number*
- PSoC COM ports have auto-generated Windows instance IDs (contain `&`)
- Real serial is on the parent USB device
- Fixed via `CM_Get_Parent()` + `CM_Get_Device_IDA()` — walks up one level in the device tree

The refactor wasn't just a port of existing FTDI code — it added support for PSoC and PIC32CX platforms that previously required the full legacy stack. That included writing platform-identification fallback logic (not all PSoC firmware implements the "Get Platform ID" command), fixing a Windows-specific serial number quirk where composite USB devices report an auto-generated COM port instance ID instead of the real device serial, and handling the subtleties of the PSoC command protocol including the `waitForCompletion` race condition in `setPinState`.

---

## Appendix F — Build Architecture

**Content:**

```
CMake build graph:

  qtac-core (static lib)          ← zero Qt dependency
      │
      ├── TACDev.dll (C API)      ← zero Qt dependency
      │
      └── qt-adapter (static lib) ← Qt6::Core only
              │
              └── qtac-app.exe    ← Qt6::Core + Qt6::Widgets + Qt6::SerialPort
```

*External dependencies of `qtac-core`:*
- `libserialport` (vendored, serial port abstraction)
- `nlohmann/json` (vendored, header-only)
- `ftd2xx.lib` / `ftd2xx.dll` (FTDI SDK, from `__Builds/x64/`)
- C++20 standard library

The build graph is intentionally a DAG with the Qt-free library at the bottom. `qtac-core` can be built and tested independently of Qt. `TACDev.dll` builds directly from `qtac-core` with no other dependencies. The `qt-adapter` bridge only needs `Qt6::Core` — not `Qt6::Widgets` — which keeps it lightweight. The full GUI app is at the top of the graph and uses all layers. This means TACDev.dll can be deployed to lab machines that have no Qt installed whatsoever.

---

## Appendix G — Licensing Considerations

### Summary

| Dependency | License | How used | Key obligation |
|------------|---------|----------|----------------|
| **nlohmann/json** | MIT | Header-only, compiled into `qtac-core` | Include copyright notice in distributed binaries |
| **libserialport** | LGPL v3 | Source fetched at build time, compiled as **static** lib into `qtac-core` | See LGPL static-link obligations below |
| **Qt6** | LGPL v3 (open-source) | Dynamic-linked — GUI app only, not in `qtac-core` or `TACDev.dll` | Include Qt copyright notice; allow user to relink against modified Qt |
| **FTDI D2XX (ftd2xx.dll)** | FTDI proprietary | Dynamic-linked at runtime | Redistribution permitted per FTDI D2XX license terms; cannot modify |

---

### nlohmann/json — MIT License

**License:** MIT (SPDX: `MIT`)
**Copyright:** © 2013–2023 Niels Lohmann

MIT is the most permissive common open-source license. The only obligation is to **include the copyright notice** in any distribution of the compiled software (e.g., in a `NOTICES` or `THIRD_PARTY_LICENSES` file alongside the binary).

No copyleft, no source disclosure, no LGPL linking restrictions. Compatible with closed-source and open-source projects without restriction.

**Required action:** Include the following in any binary distribution:

```
nlohmann/json  (https://github.com/nlohmann/json)
Copyright (c) 2013-2023 Niels Lohmann
Licensed under the MIT License.
```

---

### libserialport — LGPL v3

**License:** GNU Lesser General Public License v3
**Copyright:** © 2013–2015 Martin Ling, Uwe Hermann, and other contributors (sigrok project)
**Source:** https://github.com/sigrokproject/libserialport

LGPL v3 is designed to allow use in non-GPL (including proprietary) software, but with conditions that depend on **how the library is linked**.

#### Current configuration: statically linked

`CMakeLists.txt` builds libserialport as a static library (`add_library(serialport STATIC …)`) and links it into `qtac-core`, which in turn links into `TACDev.dll`. This is **static linking** of an LGPL v3 library.

Under LGPL v3 §4, static linking is permitted provided the distributor does **one** of the following:

- **Option A — Provide relinkable object files:** Distribute the object files of the application (not libserialport itself) so that a user can relink the application against a modified version of libserialport.
- **Option B — Use a shared library mechanism:** Link libserialport as a `.dll` / `.so` at runtime instead of statically, allowing the user to replace it.

For an internal tool this is manageable, but it does add a distribution obligation. **Option B (shared/dynamic linking) is simpler to comply with** and is worth considering.

#### Recommendation

Switch libserialport to a **dynamic/shared build** (`add_library(serialport SHARED …)` or use the system-installed `.dll`/`.so`) and distribute `serialport.dll` alongside `TACDev.dll`. This satisfies LGPL v3 §4(d)(1) trivially — the user can replace `serialport.dll` with a modified version without needing any object files from the application.

If static linking is preferred for deployment simplicity (single-DLL distribution), the object-file route (Option A) must be followed: provide the compiled `.obj` files for `qtac-core` and `TACDev.dll` (not the libserialport objects) on request.

#### Required notice regardless of link mode

Any distribution must include:

```
libserialport  (https://github.com/sigrokproject/libserialport)
Copyright (C) 2013-2015 Martin Ling and contributors
Licensed under the GNU Lesser General Public License v3.
A copy of the LGPLv3 is available at https://www.gnu.org/licenses/lgpl-3.0.html
```

---

### Qt6 — LGPL v3 (dynamic, GUI layer only)

Qt6 is used only in `qtac-app.exe` and the `qt-adapter` bridge, both of which **dynamic-link** Qt DLLs at runtime. This is the straightforward LGPL v3 use case: Qt DLLs are separate files the user can replace, so the license obligations are easily met.

Qt is **not present in `qtac-core` or `TACDev.dll`** — this was the primary motivation for the refactor.

**Required actions for `qtac-app.exe` distribution:**
- Distribute the Qt DLLs (`Qt6Core.dll`, `Qt6Widgets.dll`, etc.) unmodified alongside the application
- Include Qt's copyright and LGPL v3 notice in a `NOTICES` file
- Do not prevent users from relinking against a modified Qt — in practice, this means not stripping symbols or obfuscating in ways that prevent relink

---

### FTDI D2XX (`ftd2xx.dll`)

The FTDI D2XX library (`ftd2xx.dll`) is proprietary but freely redistributable under FTDI's standard driver license. Key points:
- **Redistribution is permitted** for end products using FTDI devices
- **Modification is not permitted**
- No source is available; it is a closed-source binary
- Must be distributed unmodified; cannot be statically linked

No additional notice is required beyond standard acknowledgment that the software uses FTDI D2XX drivers.

---

### Action Items Summary

| Item | Priority | Action |
|------|----------|--------|
| Add `NOTICES` / `THIRD_PARTY_LICENSES` file to repo | High | Include nlohmann/json MIT notice, libserialport LGPL v3 notice, Qt LGPL v3 notice |
| Evaluate libserialport link mode | Medium | Switch to dynamic linking to simplify LGPL compliance, or document object-file provision process |
| Verify Qt DLL distribution | Low | Already handled by `windeployqt` in `deploy_app.ps1` — confirm LGPL notice is bundled |

---

## Appendix I — Repository Fork & Merge-Back Plan

```
github.com/qualcomm/qcom-test-automation-controller
  (public upstream — production TAC.exe / legacy QCommonConsole)
    │
    │  fork + sync point
    │
    ▼
github.com/bryantf-qc/qtac-refactor
  (private working repo — this refactor)
    │
    │  Wave commits W1–W11 + hardware fixes
    │  source/ directory built up alongside existing src/
    │
    ▼  (future — merge back as PR)
github.com/qualcomm/qcom-test-automation-controller
    │
    ├── src/           ← legacy stack, untouched (TAC.exe still works)
    └── source/        ← new Qt-free stack added alongside
          ├── library/         qtac-core (Qt-free static lib)
          ├── libraries/       qt-adapter bridge
          ├── app/             qtac-app.exe
          ├── tacdev/          TACDev.dll (Qt-free)
          └── test/            unit + hardware integration tests
```

**Merge-back strategy:**
- `source/` is a non-conflicting new directory — the PR touches nothing in `src/`
- Legacy `TAC.exe` / `QCommonConsole` remain the default production path
- Teams adopt the new Qt-free `TACDev.dll` opt-in; old DLL remains available during transition
- Once validated across all device types, `TAC.exe` can be re-targeted to `qtac-core`

---

## Appendix H — TACDev.dll Demo (Python)

*Optional demo if time allows after slide 14:*

```python
import ctypes

tacdev = ctypes.CDLL("TACDev.dll")
tacdev.TACDev_Open.restype = ctypes.c_void_p

h = tacdev.TACDev_Open(b"COM5")
tacdev.TACDev_SendCommand(h, b"POWER_ON", True)
tacdev.TACDev_Close(h)
```

No Qt installed. No GUI. ~300 KB DLL.
