# `taccl` — One-Page Quickstart

Copy-paste, in order. Everything is Windows / `cmd.exe`.

Absolute paths assume the repo lives at:
```
C:\Users\shamazab\OneDrive - Qualcomm\Desktop\Github_repos\qtac-cli-main\qtac-cli-main
```
Adjust if yours differs.

---

## Step 1 — Install tooling (~45–90 min total, run once)

### 1a. Open an **elevated** cmd.exe
Press **Win** → type `cmd` → right-click **Command Prompt** → **Run as administrator** → click **Yes** on UAC.

You should see `C:\WINDOWS\system32>` as the prompt. That confirms it's elevated.

### 1b. Paste these one at a time and click **Yes** on any UAC dialog

```bat
winget install --id Kitware.CMake --exact --accept-source-agreements --accept-package-agreements
```
Wait for `Successfully installed`. ~1 min.

```bat
winget install --id Microsoft.VisualStudio.2022.BuildTools --exact --accept-source-agreements --accept-package-agreements --override "--wait --passive --add Microsoft.VisualStudio.Workload.VCTools --add Microsoft.VisualStudio.Component.Windows11SDK.22621 --includeRecommended"
```
Wait for `Successfully installed`. **This one takes 20–40 min** — it's downloading several GB. Do the next step (Qt) in parallel while this runs.

### 1c. Install Qt 6 (interactive — separate window)

While Build Tools is installing:

1. In a browser go to <https://www.qt.io/download-qt-installer-oss>.
2. Click **Download the Qt Online Installer**.
3. Sign in (or create a free Qt account — a personal email is fine).
4. Run the installer, sign in.
5. On "Installation folder" leave the default: `C:\Qt`.
6. On "Select Components", expand **Qt 6.9.0** (or whatever the latest 6.x is) and tick:
   - **MSVC 2022 64-bit**
   - Expand **Additional Libraries** and tick **Qt Serial Port**
7. Click Next → Next → Install. ~10–20 min.

### 1d. Verify everything is installed

Close every terminal. Open a **new** elevated cmd.exe and check:

```bat
git --version
"C:\Program Files\CMake\bin\cmake.exe" --version
dir "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC"
dir "C:\Qt\6.9.0\msvc2022_64\bin\Qt6Core.dll"
```
All four should succeed. (Your Qt version number may differ — adjust the path.)

---

## Step 2 — Open the right shell for building

From the **Start menu**, search for and open:

**`x64 Native Tools Command Prompt for VS 2022`**

(Not regular cmd.exe. This one has `cl.exe`, `MSBuild`, and the Windows SDK on PATH.)

Verify:
```bat
cl        REM should print "Microsoft (R) C/C++ Optimizing Compiler..."
cmake --version
```

---

## Step 3 — Build the parent repo (produces `TACDev.dll`) ~10 min

Still in the **x64 Native Tools Command Prompt**:

```bat
cd /d "C:\Users\shamazab\OneDrive - Qualcomm\Desktop\Github_repos\qtac-cli-main\qtac-cli-main"

set CMAKE_PREFIX_PATH=C:\Qt\6.9.0\msvc2022_64

build.bat
```

If `build.bat` fails or doesn't exist:
```bat
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

Confirm the DLL was built:
```bat
where /r build TACDev.dll
where /r build TACDev.lib
```
Note whichever folder they're in (usually `build\interfaces\C++\TACDev\Release`).

---

## Step 4 — Build `taccl` ~2 min

Same terminal:

```bat
cd tools\taccl

cmake -B build ^
      -DTACDEV_LIBRARY_DIR="..\..\build\interfaces\C++\TACDev\Release" ^
      -DTACDEV_LIBRARY_NAME=TACDev

cmake --build build --config Release
```

Result: `tools\taccl\build\Release\taccl.exe`. First configure downloads CLI11 + nlohmann/json from GitHub (needs internet).

---

## Step 5 — Copy the DLLs next to `taccl.exe`

```bat
cd build\Release

copy ..\..\..\..\build\interfaces\C++\TACDev\Release\TACDev.dll .
copy "C:\Qt\6.9.0\msvc2022_64\bin\Qt6Core.dll"       .
copy "C:\Qt\6.9.0\msvc2022_64\bin\Qt6SerialPort.dll" .

REM Find ftd2xx.dll (parent repo ships it under third-party)
where /r ..\..\..\..\ ftd2xx.dll
REM Copy from the path that printed. Example:
REM copy ..\..\..\..\third-party\ftdi\amd64\ftd2xx.dll .
```

---

## Step 6 — Live demo commands

Still in `tools\taccl\build\Release`:

### Without a TAC board plugged in (proves it runs)

```bat
taccl --version
taccl --help
taccl battery --help

taccl battery --device=COM99
echo %errorlevel%                          REM should be 2

taccl battery --device=COM99 --json        REM {"error":"device 'COM99' not found"}

taccl battery --device=COM3 --state=bogus  REM exit 3 - CLI11 rejects
```

### With a TAC board plugged in (find its COM port in Device Manager)

```bat
set PORT=COM3        REM replace with your actual port

taccl battery --device=%PORT%
taccl battery --device=%PORT% --state=off
taccl battery --device=%PORT%              REM prints "off"
taccl battery --device=%PORT% --state=on

taccl usb0        --device=%PORT%
taccl power-key   --device=%PORT% --state=on
taccl power-key   --device=%PORT% --state=off

taccl battery --device=%PORT% --json       REM {"state":true}

taccl external-power --device=%PORT% --state=on
taccl pin            --device=%PORT% --pin=7 --state=on
```

---

## If something breaks

| Symptom | Fix |
|---|---|
| `winget install` prints nothing / exits silently | You forgot admin. Reopen cmd.exe as administrator. |
| `cl` not recognized | You're in a plain cmd.exe. Open **x64 Native Tools Command Prompt for VS 2022**. |
| CMake "Qt6::Core not found" | Set `CMAKE_PREFIX_PATH` to the exact Qt install path before running `cmake -B build`. |
| `taccl.exe` starts and immediately dies | A DLL is missing next to it. Re-do Step 5. |
| `taccl battery ...` returns exit 2 | Wrong COM port. Check Device Manager. |
| `taccl` returns exit 5 | FTDI driver not installed. Get FTDI D2XX driver from ftdichip.com. |

---

## Time budget

| Phase | Time |
|---|---|
| Step 1 (installs) | 45–90 min |
| Step 2 (open shell) | 30 sec |
| Step 3 (build parent) | 5–15 min |
| Step 4 (build taccl) | 1–3 min |
| Step 5 (copy DLLs) | 1 min |
| Step 6 (demo) | as long as you want |

Total from cold start: **~1.5 hours** if downloads are fast, **~2 hours** on slower connections.