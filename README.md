# Qualcomm Test Automation Controller (QTAC)

[![CMake Build](https://github.com/qualcomm/qcom-test-automation-controller/actions/workflows/build.yml/badge.svg)](https://github.com/qualcomm/qcom-test-automation-controller/actions/workflows/build.yml)

## Table of Contents
- [Introduction](#introduction)
- [What You Get](#what-you-get)
- [Hardware Requirements](#hardware-requirements)
- [Common Prerequisites](#common-prerequisites)
- [Windows Guide](#windows-guide)
- [Linux Guide](#linux-guide)
- [Repository Structure](#repository-structure)
- [Application Dependency Architecture](#application-dependency-architecture)
- [Advanced Topics](#advanced-topics)
- [Support & Contributing](#support--contributing)

## Introduction

QTAC is a software suite that enables users to control Qualcomm devices remotely via a Qualcomm approved debug board connected to a host.

## What You Get

| Application | Description |
| :--- | :--- |
| **Test Automation Controller (TAC)** | Control Qualcomm devices remotely using UI |
| **TAC Configuration Editor** | Design configurable TAC UI based on GPIO use-cases |
| **Device Catalog** | Program debug board and view supported Qualcomm platforms |
| **Command-line utilities** | DevList, FTDICheck, LITEProgrammer, PSOCProgrammer, TACDump, UpdateDeviceList |

![QTAC UI Overview](./docs/resources/qtac-sw-suite.png)

## Hardware Requirements

**Required Hardware**:
- Qualcomm approved debug board (FTDI, Cypress PSoC5LP or Microchip PIC32CX SG41-based)
- Qualcomm device to be controlled
- USB Cables: Type B Micro-USB (Board to Host) & Type-C (Device to Host)

**Setup**: Connect the device to the debug board (directly or via cable strip) and both to the host.

> [!IMPORTANT]
> PSOC debug boards must be factory-programmed with firmware. If the board isn't recognized, it likely lacks programming.

![QTAC Hardware Software Setup](./docs/resources/qtac-setup-overview.drawio.png)

## Common Prerequisites

### Development Tools

| Category | Software | Minimum Version |
| :-- | :-- | :-- |
| **OS** | Windows / Debian | Windows 10+ / Ubuntu 22.04+ |
| **Compiler** | [MSVC 2022](https://aka.ms/vs/17/release/vs_community.exe) / GCC | MSVC 2022 / GCC-11, G++-11, GLIBC-2.35 |
| **Build System** | [CMake](https://cmake.org/download/) | 3.22+ |
| **UI Framework** | [Qt Open-source](https://www.qt.io/download-qt-installer-oss) | 6.9.0+ |

> [!NOTE]
> Review license terms for [Visual Studio](https://visualstudio.microsoft.com/license-terms/) and [Qt](https://www.qt.io/development/download-open-source). MSVC 2022 is linked as Qt doesn't support MSVC 2026 yet.

### Drivers

- **[FTDI D2XX drivers](https://ftdichip.com/drivers/d2xx-drivers/)**: For debug board interaction.
- **[Qualcomm USB Drivers](https://softwarecenter.qualcomm.com/catalog/item/Qualcomm_Userspace_Driver)**: To view device status.

> [!NOTE]
> FTDI libraries are downloaded _automatically_ during the cmake configuration step when building from source.
> If the automatic download fails (e.g. due to network restrictions), download the archive manually and place it in the `third-party/` directory before re-running cmake:
>
> | Platform | Archive | URL |
> | :-- | :-- | :-- |
> | **Windows x64** | `CDM-v2.12.36.4-WHQL-Certified.zip` | https://web.archive.org/web/20250820134143/https://ftdichip.com/wp-content/uploads/2023/09/CDM-v2.12.36.4-WHQL-Certified.zip |
> | **Windows ARM64** | `CDM-v2.12.36.20-for-ARM64-WHQL-Certified.zip` | https://web.archive.org/web/20250821211500/https://ftdichip.com/wp-content/uploads/2025/03/CDM-v2.12.36.20-for-ARM64-WHQL-Certified.zip |
> | **Linux x86_64** | `libftd2xx-linux-x86_64-1.4.33.tgz` | https://web.archive.org/web/20250822044524/https://ftdichip.com/wp-content/uploads/2025/03/libftd2xx-linux-x86_64-1.4.33.tgz |

### Optional Software

QTAC allows you to view streaming device logs as you transition the device between different states. The debug logs are streamed over USB serial interface(s).

To view these logs, you may install [Putty](https://www.putty.org/) or similar terminal software. QTAC does not depend on or use this software.

### Clone Repository

```bash
git clone https://github.com/qualcomm/qcom-test-automation-controller.git
```

## Windows Guide

### Configuration

1. **Visual Studio**: Install **Desktop development with C++** and **.NET desktop development**.
   ![Desktop development with C++](./docs/resources/qtac-msvc-2022-requirements.png)
2. **Qt**: Install Qt 6.9+ for **MSVC 2022 64-bit**, **Qt Serial Port** and **Qt Multimedia** components.
   For ARM64, also install the **MSVC 2022 ARM64** Qt component.
   
> [!NOTE]
> Installation using Qt Online Installer will require users to create a Qt account.
3. **Environment Variable**:

   **x64**:
   ```cmd
   setx QTBIN C:\Qt\<version>\msvc2022_64\bin
   ```
   **ARM64**:
   ```cmd
   setx QTBIN C:\Qt\<version>\msvc2022_arm64\bin
   ```

### Build & Usage

Execute `build.bat` to generate executables:

```cmd
build.bat        (x64, default)
build.bat ARM64  (ARM64)
```

**Build output**:
- x64 Debug: `__Builds\x64\Debug`
- x64 Release: `__Builds\x64\Release`
- ARM64 Debug: `__Builds\ARM64\Debug`
- ARM64 Release: `__Builds\ARM64\Release`

**Usage**:
```cmd
__Builds\x64\Release\QTAC.exe
```

## Linux Guide

### Configuration

> [!IMPORTANT]
> - Installation using Qt Online Installer will require users to create a Qt account.
> - If you're frequently working with Qt on Linux, consider adding the environment variables to `.bashrc`.
> - Using `sudo apt install <package>` will update setup packages. Review command usage to prevent issues with other applications.

1. **Qt Installation** (choose one):
   
   **Option A**: Qt Online Installer
   - Install Qt 6.9+ for **GCC 64-bit** and **Qt Serial Port** and **Qt Multimedia** components using [Qt Online Installer](https://www.qt.io/download-qt-installer-oss)
   
   **Option B**: Quick Installation via apt
   ```bash
   sudo apt install qt6-base-dev qt6-serialport-dev qt6-multimedia-dev
   ```
2. **Runtime Dependencies**:
   ```bash
   sudo cp udev-rules/99-QTAC-USB.rules /etc/udev/rules.d/
   sudo udevadm control --reload
   ```
4. **Environment Variable**:
   ```bash
   export QTBIN=/path/to/Qt/directory/<version>/gcc_64/bin
   ```

### Build & Usage

Execute `build.sh` to generate executables:

```bash
./build.sh
```

**Build output**:
- Debug: `__Builds/Linux/Debug`
- Release: `__Builds/Linux/Release`

> [!NOTE]
> Ensure that [make](https://www.gnu.org/software/make/) is available in your environment before building.

**Usage**:
```bash
./__Builds/Linux/Release/QTAC
```

## Repository Structure

| Directory | Content |
| :-- | :-- |
| `.github` | CI/CD build pipelines |
| `configurations` | Platform-specific pin configurations |
| `docs` | Documentation and guides |
| `examples` | Device-control automation scripts |
| `interfaces` | APIs for C++, Python, C#, and Java |
| `src` | Source files (Applications & Libraries) |
| `third-party` | External dependency scripts |

## Application Dependency Architecture

![App dependency](./docs/resources/app-dependency.drawio.png)

## Advanced Topics

- [Build Using Qt Creator](./docs/getting-started/01-Build-Using-Qt-Creator.md)
- [Python API Guide](./docs/bootcamp/01-Bootcamp.md)

## Support & Contributing

- **Security**: Review [SECURITY.md](./.github/SECURITY.md) for vulnerability reporting.
- **Contributing**: Review [License](./LICENSE) and [Code of Conduct](./CODE-OF-CONDUCT.md).
