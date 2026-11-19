# CLI/GUI Tools Migration Plan: `src/applications/` → `source/applications/`

## Overview

Migrate all applications from `src/applications/` into the `source/` tree.
CLI tools will have their Qt dependencies removed. GUI tools will keep Qt but
switch from QCommonConsole to `qtac-core` + `qt-adapter`.

---

## Tools Summary

| Tool | Type | Phase | What Will Be Done |
|---|---|---|---|
| `devlist` | CLI | 2a | Remove Qt/QCommonConsole. Replace `USBDescriptors` with `PlatformContainer::getEntries()`. Replace `QCommandLineParser` with `qtac::ArgParser`. |
| `tacdump` | CLI | 2b | Remove Qt/QCommonConsole. Replace Qt string types with `qtac::` equivalents. Keep `_AlpacaDevice` enumeration APIs (already in `source/library`). |
| `lite-programmer` | CLI | 2c | Remove Qt/QCommonConsole. Replace Qt string types with `qtac::` equivalents. Keep `FTDIDevice::programDevice()` (already in `source/library`). |
| `updatedevicelist` | CLI | 2d | Remove Qt/QCommonConsole. Replace `QDir` with `std::filesystem`. Replace `QMap` with `std::map`. Use `boost::json` for JSON output. Keep `_FTDIPlatformConfiguration` (already in `source/library`). |
| `ftdi-check` | CLI | 2e | Remove Qt/QCommonConsole (the only QCommonConsole usage is init boilerplate). Replace `QProcess` with `popen()`/`_popen()`, `QSettings` INF parsing with `std::ifstream`, `QFile`/`QDir` with `std::filesystem`. All Win32 API calls unchanged. |
| `device-catalog` | GUI | 3 | Move to `source/applications/gui/`. Keep Qt6 Widgets/Gui. Replace QCommonConsole with `qtac-core` + `qt-adapter`. |
| `tac-configuration-editor` | GUI | 3 | Move to `source/applications/gui/`. Keep Qt6 Widgets/Gui. Replace QCommonConsole with `qtac-core` + `qt-adapter`. |
| `test-automation-controller` | GUI | 3 | Move to `source/applications/gui/`. Keep Qt6 Widgets/Gui. Replace QCommonConsole with `qtac-core` + `qt-adapter`. |

---

## Target Directory Structure

```
source/applications/
  CMakeLists.txt                  ← includes cli/ and gui/
  cli/
    CMakeLists.txt
    devlist/
    tacdump/
    updatedevicelist/
    ftdi-check/
    lite-programmer/
  gui/
    CMakeLists.txt
    device-catalog/               ← Qt kept; QCommonConsole → qtac-core + qt-adapter
    tac-configuration-editor/     ← Qt kept; QCommonConsole → qtac-core + qt-adapter
    test-automation-controller/   ← Qt kept; QCommonConsole → qtac-core + qt-adapter
```

`source/CMakeLists.txt` gains `add_subdirectory(applications)`. CLI targets are
always built; GUI targets are gated on the existing `QTAC_BUILD_QT_TARGETS` option.

---

## Qt → stdlib/qtac Replacement Table (CLI tools)

| Qt type | Replacement |
|---|---|
| `QCoreApplication` | Remove entirely (no event loop needed) |
| `QString` | `qtac::String` |
| `QStringList` | `qtac::StringList` |
| `QByteArray` | `qtac::ByteArray` |
| `QCommandLineParser` / `QCommandLineOption` | New `qtac::ArgParser` (see Phase 1) |
| `QDir` / `QFileInfo` | `std::filesystem::path` / `directory_iterator` |
| `QMap<K,V>` | `std::map<K,V>` |
| `QProcess` | `popen()` / `_popen()` |
| `QSettings` (INF parsing) | `std::ifstream` custom line parser |
| `QFile` | `std::ifstream` / `std::ofstream` / `std::filesystem::copy` |

---

## Phase 1 — Add Two Helpers to `source/library` (~1 day)

### 1a. `qtac::ArgParser`
**File:** `source/library/include/qtac/ArgParser.h` (header-only)

Minimal replacement for `QCommandLineParser`. Parses `argc`/`argv` with named
options (e.g. `--output`, `-v`), positional arguments, and `--help` generation.
Used by all five CLI tools.

### 1b. Config Root Path
**File:** `source/library/include/qtac/ConfigPath.h` (header-only)

`qtac::configRoot()` — returns the directory path containing `devicelist.json`.
Resolution order:
1. `QTAC_CONFIG_PATH` environment variable
2. Exe-relative `../configurations/`
3. Current working directory

Mirrors the existing tcnf resolution logic in `PlatformID.cpp`.

---

## Phase 2 — CLI Tools

Tools are ordered from simplest to most complex.

### 2a. `devlist` (~0.5 days, ~60 lines)

**What it does:** Reads `devicelist.json` and pretty-prints all registered
platform entries (name, board type, platform ID, USB descriptor, revision,
config file path). Does **not** enumerate live hardware.

**Port strategy:**
- Remove `QCoreApplication`, `USBDescriptors`, `tacConfigRoot()`
- Call `PlatformContainer::initialize(qtac::configRoot() + "devicelist.json")`
  — already parses the same JSON
- Iterate `PlatformContainer::getEntries()` — `_PlatformEntry` already carries
  `_description`, `_platformID`, `_debugBoardType`, `_usbDescriptor`,
  `_revision`, `_path`
- Table output via `std::setw` (unchanged)
- Replace `-quts` flag handling with `qtac::ArgParser`

**Blockers:** Phase 1

---

### 2b. `tacdump` (~0.5 days, ~160 lines)

**What it does:** Enumerates all currently connected TAC devices and prints
port name and serial number. Supports a `-quts` machine-readable output format.

**Port strategy:**
- Remove `QCoreApplication`, `AppCore`, `TACPreferences`,
  `InitializeQCommonConsole()`
- Replace `QCommandLineParser` → `qtac::ArgParser`
- Replace `QString` / `QStringList` → `qtac::String` / `qtac::StringList`
- Keep `_AlpacaDevice::updateAlpacaDevices()` and `getAlpacaDevices()` — both
  already in `source/library`

**Blockers:** Phase 1

---

### 2c. `lite-programmer` (~1 day, ~315 lines)

**What it does:** Programs FTDI debug boards with a platform ID, identified by
serial number. Can also list available FTDI devices and registered platforms.

**Port strategy:**
- Remove `QCoreApplication`, `AppCore`, `InitializeQCommonConsole()`
- Replace `QString` / `QByteArray` → `qtac::String` / `qtac::ByteArray`
- Replace `QCommandLineParser` → `qtac::ArgParser`
- Keep `FTDIDevice::programDevice()`,
  `PlatformContainer::getDebugBoardsOfType(eFTDI)`, and `_AlpacaDevice`
  enumeration APIs — all already in `source/library` with matching signatures

**Blockers:** Phase 1

---

### 2d. `updatedevicelist` (~2 days, ~310 lines)

**What it does:** Scans a directory for `.tcnf` platform configuration files,
loads each one, collects USB descriptor metadata, and writes `devicelist.json`.
Run as a post-install step on Linux — **must work without Qt**.

**Port strategy:**
- Remove `QCoreApplication`, `AppCore`, `InitializeQCommonConsole()`
- Replace `QDir::entryInfoList()` → `std::filesystem::directory_iterator`
  filtered to `.tcnf` extension
- Replace `QMap<PlatformID, USBDescriptor>` → `std::map<PlatformID, ...>`
- Replace `QString` / `QStringList` → `qtac::String` / `qtac::StringList`
- Replace `QCommandLineParser` → `qtac::ArgParser`
- Keep `PlatformContainer::getDebugBoards()` and `_FTDIPlatformConfiguration`
  — already in `source/library`
- JSON output: use `boost::json` (already vendored) to write `devicelist.json`,
  preserving the existing format

**Blockers:** Phase 1

---

### 2e. `ftdi-check` (~3 days, ~720 lines)

**What it does:** Validates that the FTDI D2XX driver is correctly installed,
uninstalls old versions, and installs/repairs the expected version
(CDM 2.12.36.4 on Windows, 1.4.27 on Linux). Architecture-aware (x64/ARM).

**Port strategy:**
- Remove `QCoreApplication`, `AppCore`, `InitializeQCommonConsole()` — the
  only QCommonConsole usage; the rest of the tool is platform-specific code
- Replace `QProcess` → a small `runProcess(cmd) → string` helper using
  `popen()` / `_popen()` to capture stdout (used for `pnputil` output)
- Replace `QSettings` INF parsing → custom `std::ifstream` parser for the
  `[Version]` section (only reads `DriverVer=` and `CatalogFile=` lines)
- Replace `QDir` / `QFile` → `std::filesystem::path`,
  `std::filesystem::copy`, `std::filesystem::create_symlink`
- Replace `QString` / `QStringList` / `QMap` →
  `std::string` / `std::vector<std::string>` / `std::map<std::string, std::string>`
- All Win32 API calls (`LoadLibraryA`, `EnumDeviceDrivers`, `IsWow64Process2`,
  etc.) are unchanged — already platform-specific behind `#ifdef _WIN32`

**Blockers:** Phase 1

---

## Phase 3 — GUI Tools (deferred)

`device-catalog`, `tac-configuration-editor`, and `test-automation-controller`
keep their Qt6 Widgets/Gui dependencies but replace QCommonConsole with
`qtac-core` + `qt-adapter`.

These applications are significantly larger (~5–10k lines each) and are
recommended as a follow-on effort after the CLI tools are complete.

---

## Phase 4 — CMake Integration (~0.5 days)

**`source/applications/cli/CMakeLists.txt`**
- Each CLI target links: `qtac-core` + system libs only (no Qt)
- `ftdi-check` and `lite-programmer` additionally link `ftd2xx`, `SetupAPI`,
  `psapi` (Windows)

**`source/applications/gui/CMakeLists.txt`**
- Each GUI target links: `qtac-core` + `qt-adapter` + `Qt6::{Core,Widgets,Gui,...}`
- Gated on `QTAC_BUILD_QT_TARGETS`

**`source/CMakeLists.txt`**
- Add `add_subdirectory(applications)`

---

## Phase 5 — Packaging & CI (~0.5 days)

- **`packaging/linux/package_QTAC.sh`:** update `BinDir` to point at new build
  output; ensure the `postinst` `UpdateDeviceList` invocation uses the new path
- **`packaging/windows/build-installer.ps1`:** update binary directory path
- **`.github/workflows/build.yml`:** `build-source-*` jobs already invoke
  `cmake -S source`; add install/copy steps for the new CLI binaries

---

## Out of Scope

- **`psoc-programmer`** — currently not built in `src/` either; defer
- **GUI tools** — deferred to Phase 3 above; `src/` versions remain until then

---

## Effort Summary

| Item | Effort |
|---|---|
| Phase 1 — ArgParser + configRoot helpers | 1 day |
| devlist | 0.5 day |
| tacdump | 0.5 day |
| lite-programmer | 1 day |
| updatedevicelist | 2 days |
| ftdi-check | 3 days |
| Phase 4 — CMake + Phase 5 — Packaging/CI | 0.5 day |
| **Total (CLI + infrastructure)** | **~8.5 days** |
