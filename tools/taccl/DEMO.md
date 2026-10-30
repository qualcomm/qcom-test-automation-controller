# `taccl` — Live Demo Playbook

This file is the "do this tomorrow" script. It covers three things:

1. **Install** all dependencies on a Windows dev box from scratch.
2. **Build** the parent repo and then `taccl` on top of it.
3. **Run** a live demo with the repo owner — three shapes depending on what
   hardware/time you have.

Everything is Windows / `cmd.exe`. If you're in PowerShell adjust quoting.

---

## 0. What you need on the machine

| # | Dependency | Approx size | Why |
|---|---|---|---|
| 1 | Git for Windows | 250 MB | Cloning + CMake's `FetchContent` uses git |
| 2 | CMake ≥ 3.16 | 100 MB | Build system generator |
| 3 | Visual Studio 2022 Build Tools + C++ workload | ~6–8 GB | Provides `cl.exe`, `MSBuild`, Windows SDK |
| 4 | Qt 6 (`Core` + `SerialPort` for MSVC 2022) | ~3–5 GB | The parent repo (not `taccl` itself) needs Qt to build `TACDev.dll` |
| 5 | A TAC hardware board on USB | – | For any real command (skippable for `--help` / `--version`) |

`taccl` itself only depends on 1, 2, 3. It's Qt-free by design. Qt is only
needed to build the DLL it links against.

---

## 1. Install dependencies

### 1a. Non-interactive stuff via `winget`

Open **cmd.exe as Administrator** (right-click Start → Terminal (Admin), or
Win+X → Terminal (Admin)). Then paste:

```bat
winget install --id Git.Git                  --exact --accept-source-agreements --accept-package-agreements
winget install --id Kitware.CMake            --exact --accept-source-agreements --accept-package-agreements
winget install --id Microsoft.VisualStudio.2022.BuildTools --exact --accept-source-agreements --accept-package-agreements --override "--wait --passive --add Microsoft.VisualStudio.Workload.VCTools --add Microsoft.VisualStudio.Component.Windows11SDK.22621 --includeRecommended"
```

The Build Tools install takes a while (10–30 min). When it finishes, **close
and re-open** the terminal so new PATH entries take effect.

Verify:

```bat
git --version
cmake --version
where cl
```

`where cl` may still fail from a plain shell — MSVC's `cl.exe` is only on
PATH inside a "Developer Command Prompt". You'll use that for the actual
build (see step 2).

### 1b. Qt — interactive installer

Qt does not have a great silent install. Use the online installer:

1. Grab it from <https://www.qt.io/download-qt-installer-oss>.
2. Sign in / create a free Qt account (required for the OSS installer).
3. When it asks what to install, pick:
   - **Qt 6.9.x** (or whatever the parent repo uses — check
     `qtac-cli-main/CMakeLists.txt` for the exact version).
   - Under that version, tick **MSVC 2022 64-bit**.
   - Under that entry's "Additional Libraries", tick **Qt Serial Port**.
4. Let it install (typically to `C:\Qt\`).

Note the install path — you'll pass it as `CMAKE_PREFIX_PATH` in step 2.
Typical path is `C:\Qt\6.9.0\msvc2022_64`.

### 1c. Sanity re-check

Open a fresh **x64 Native Tools Command Prompt for VS 2022** (Start menu →
"x64 Native Tools"). This is the shell where `cl.exe` and `MSBuild` are on
PATH.

```bat
cl               2>nul
cmake --version
git --version
dir C:\Qt\6.9.0\msvc2022_64\bin\Qt6Core.dll
```

All four should succeed.

---

## 2. Build the parent repo (produces `TACDev.dll`)

From the **x64 Native Tools Command Prompt for VS 2022**:

```bat
cd /d "C:\Users\shamazab\OneDrive - Qualcomm\Desktop\Github_repos\qtac-cli-main\qtac-cli-main"

set CMAKE_PREFIX_PATH=C:\Qt\6.9.0\msvc2022_64

REM Use the repo's own build script if it exists, otherwise:
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

If the repo ships a `build.bat`, prefer that:

```bat
build.bat
```

When it finishes, locate the produced TACDev files:

```bat
where /r build TACDev.dll TACDev.lib
```

Note the folder (call it `TACDEV_OUT`). Typical:
`build\interfaces\C++\TACDev\Release`.

---

## 3. Build `taccl` on top

Same **x64 Native Tools Command Prompt for VS 2022**:

```bat
cd tools\taccl

cmake -B build ^
      -DTACDEV_LIBRARY_DIR="..\..\build\interfaces\C++\TACDev\Release" ^
      -DTACDEV_LIBRARY_NAME=TACDev

cmake --build build --config Release
```

Result: `tools\taccl\build\Release\taccl.exe`.

First run of `cmake -B build` will fetch **CLI11** and **nlohmann/json** from
GitHub via `FetchContent`. Needs internet **once**.

---

## 4. Make the DLLs findable

`taccl.exe` loads `TACDev.dll` at runtime, which in turn loads its Qt and
FTDI dependencies. Easiest fix — copy everything next to `taccl.exe`:

```bat
cd tools\taccl\build\Release

copy ..\..\..\..\build\interfaces\C++\TACDev\Release\TACDev.dll .

REM Qt DLLs
set QTBIN=C:\Qt\6.9.0\msvc2022_64\bin
copy "%QTBIN%\Qt6Core.dll"       .
copy "%QTBIN%\Qt6SerialPort.dll" .

REM FTDI (comes with the parent repo's third-party dir)
where /r ..\..\..\..\ ftd2xx.dll
copy <path-that-where-just-printed>\ftd2xx.dll .
```

---

## 5. The live demo

You have three demo shapes. Pick whichever your situation supports.

### Shape A — Design / code walkthrough only (needs nothing)

Best if you're just presenting to the owner and won't actually run the tool.

1. Open the repo in VS Code.
2. Show `docs/taccl-plan.md`, scroll to **Section 5**.
3. Open `qtac-cli-main/tools/taccl/`:
   - **`README.md`** — headline summary.
   - **`StateControls.cpp`** — point at the `kStateControls` table, explain
     one row (say `battery`), explain how one dispatcher (`runStandardControl`)
     handles all 15 standard rows, and the two hand-written specials
     (`external-power`, `pin`).
   - **`main.cpp`** — show that it's a thin entry point; sections 6-9 will
     each add one more `Register*()` call.
   - **`CMakeLists.txt`** — highlight "no Qt, only links `TACDev`".
4. Talk through the parity story: same `taccl.exe` + swap `TACDev.dll` =
   apples-to-apples Qt-vs-non-Qt comparison.

### Shape B — Compiles-and-runs demo (needs steps 1–4, no board)

Prove the pipeline is real, even without hardware.

```bat
cd tools\taccl\build\Release

taccl --version
```

Expected: prints `taccl 0.1.0 (Section 5 - State Controls)` and the QTAC /
TAC library versions (proves `TACDev.dll` loaded cleanly).

```bat
taccl --help
```

Expected: top-level help listing every Section 5 subcommand (`battery`,
`usb0`, `usb1`, `power-key`, `volume-up`, `volume-down`, `disconnect-uim1`,
`disconnect-uim2`, `disconnect-sdcard`, `disconnect-headset`, `primary-edl`,
`secondary-edl`, `force-pshold`, `secondary-pm-resin`, `eud`,
`external-power`, `pin`).

```bat
taccl battery --help
taccl pin     --help
```

Expected: per-subcommand help showing `--device`, `--serial`, `--state`
(and `--pin` on `pin`).

```bat
taccl battery --device=COM99
echo %errorlevel%
```

Expected: `taccl: device 'COM99' not found` on stderr, exit code `2`.
That's the "device-not-found" error path working end to end.

```bat
taccl battery --device=COM99 --json
```

Expected: `{"error":"device 'COM99' not found"}` on stderr.
Proves JSON error formatting.

```bat
taccl battery --device=COM3 --state=xxx
```

Expected: CLI11 rejects `xxx`, exit code `3`, help printed on stderr.
Proves argument validation.

### Shape C — Full live demo (needs everything + a TAC board)

Plug in a TAC device, find its port (e.g. from Device Manager), then run:

```bat
set PORT=COM3

REM 1) Query current battery state
taccl battery --device=%PORT%
echo %errorlevel%                                    REM 0

REM 2) Toggle it and read back
taccl battery --device=%PORT% --state=off
taccl battery --device=%PORT%                        REM prints off
taccl battery --device=%PORT% --state=on

REM 3) A few other controls to show table coverage
taccl usb0      --device=%PORT%
taccl power-key --device=%PORT% --state=on
taccl power-key --device=%PORT% --state=off
taccl volume-up --device=%PORT%

REM 4) JSON
taccl battery --device=%PORT% --json                 REM {"state":true}

REM 5) Lookup by serial (grab the serial from the parent app's device list)
set SERIAL=<your-device-serial>
taccl battery --serial=%SERIAL%

REM 6) Environment fallback
set TACCL_DEVICE=%PORT%
taccl battery

REM 7) Set-only special cases
taccl external-power --device=%PORT% --state=on
taccl pin            --device=%PORT% --pin=7 --state=on

REM 8) Error paths
taccl battery --device=COM99                         REM exit 2
taccl external-power --device=%PORT%                 REM exit 3 (set-only)
```

### Shape D — Parity demo (the whole reason the CLI exists)

Only meaningful if you can get *both* the Qt and non-Qt builds of
`TACDev.dll`. On a TAC-equipped machine:

```bat
REM Round 1 - Qt build
copy path\to\qt-build\TACDev.dll   .
taccl battery   --device=%PORT% > qt.log
taccl usb0      --device=%PORT% >> qt.log
taccl power-key --device=%PORT% >> qt.log

REM Round 2 - non-Qt build
copy path\to\non-qt-build\TACDev.dll   .    REM overwrite
taccl battery   --device=%PORT% > non-qt.log
taccl usb0      --device=%PORT% >> non-qt.log
taccl power-key --device=%PORT% >> non-qt.log

fc qt.log non-qt.log
```

`No differences encountered` = parity across both TAC builds for those
Section 5 operations.

---

## 6. Troubleshooting

| Symptom | Cause | Fix |
|---|---|---|
| `cl` not found | Wrong shell | Use **x64 Native Tools Command Prompt for VS 2022** |
| `Qt6::Core` not found by CMake | `CMAKE_PREFIX_PATH` not set | `set CMAKE_PREFIX_PATH=C:\Qt\6.9.0\msvc2022_64` before configure |
| `taccl.exe` fails on start, "…dll not found" | DLLs not next to exe | Copy `TACDev.dll`, `Qt6Core.dll`, `Qt6SerialPort.dll`, `ftd2xx.dll` next to `taccl.exe` |
| `taccl battery --device=COMx` exit 2 | Port wrong / device not plugged in | Verify port in Device Manager; try `--serial=` instead |
| Exit 5 on any command | `InitializeTACDev()` failed | Usually FTDI driver missing; install FTDI D2XX driver |
| `winget install` seems silent | UAC prompt auto-dismissed | Run from **elevated** cmd.exe (right-click → "Run as administrator") |

---

## 7. What to say to the repo owner

Short version:

> "I implemented Section 5 (State Controls) from the CLI plan doc as a
> standalone Qt-free CLI under `tools/taccl/`. It's a separate CMake
> project, links only against `TACDev.dll`, uses CLI11 for argument parsing
> and nlohmann/json for JSON output. All 17 Section 5 subcommands are wired
> up via a table so the code is very compact. Global options, device
> resolution (port/serial/env), error handling, exit codes, JSON mode and
> `--version` / `--help` are all in place. The other sections (list, info,
> set, boot, commands, vars, help-text, queue-clear, logging) haven't been
> implemented yet, but the scaffolding is done so they should each be small
> follow-ups. I have a README with a build + test walkthrough and a demo
> playbook (`DEMO.md`)."

Follow with Shape B or C above to actually show it working.