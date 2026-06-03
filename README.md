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
| **UI Framework** | [Qt Open-source](https://www.qt.io/download-qt-installer-oss) | 6.9.0+ |

> [!NOTE]
> Review license terms for [Visual Studio](https://visualstudio.microsoft.com/license-terms/) and [Qt](https://www.qt.io/development/download-open-source). MSVC 2022 is linked as Qt doesn't support MSVC 2026 yet.

### Drivers

- **[FTDI D2XX drivers](https://ftdichip.com/drivers/d2xx-drivers/)**: For debug board interaction.
- **[Qualcomm USB Drivers](https://softwarecenter.qualcomm.com/catalog/item/Qualcomm_Userspace_Driver)**: To view device status.

> [!IMPORTANT]
> The FTDI D2XX library **must be downloaded manually** before building. The FTDI website is
> protected by Cloudflare and cannot be fetched by automated tools. See the
> [build steps below](#windows-guide) or [DEPENDENCIES.md](./DEPENDENCIES.md) for instructions.

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
   
> [!NOTE]
> Installation using Qt Online Installer will require users to create a Qt account.
3. **Environment Variable**:
   ```cmd
   setx QTBIN C:\Qt\<version>\msvc2022_64\bin
   ```

### Build & Usage

**Step 1 — Download the FTDI D2XX library** (required before first build):

1. Open a browser and go to **https://ftdichip.com/drivers/d2xx-drivers/**
2. Download the latest Windows D2XX driver (`CDM-vX.XX.XX-WHQL-Certified.zip`)

**Step 2 — Build**:

```cmd
set FTDI_ARCHIVE_PATH=C:\path\to\CDM-vX.XX.XX-WHQL-Certified.zip
build.bat
```

> [!NOTE]
> `FTDI_ARCHIVE_PATH` is only needed on the first build (or after cleaning `__Builds\`).
> Subsequent builds can use `build.bat` without setting it.

**Build output**:
- Debug: `__Builds\x64\Debug`
- Release: `__Builds\x64\Release`

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
   - Install Qt 6.9+ for **GCC 64-bit** and **Qt Serial Port** component using [Qt Online Installer](https://www.qt.io/download-qt-installer-oss)
   
   **Option B**: Quick Installation via apt
   ```bash
   sudo apt install qt6-base-dev qt6-serialport-dev
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

**Step 1 — Download the FTDI D2XX library** (required before first build):

1. Open a browser and go to **https://ftdichip.com/drivers/d2xx-drivers/**
2. Download the latest Linux x86_64 D2XX library (`libftd2xx-linux-x86_64-X.X.XX.tgz`)

**Step 2 — Build**:

```bash
export FTDI_ARCHIVE_PATH=/path/to/libftd2xx-linux-x86_64-X.X.XX.tgz
./build.sh
```

> [!NOTE]
> `FTDI_ARCHIVE_PATH` is only needed on the first build (or after cleaning `__Builds/`).
> Subsequent builds can use `./build.sh` without setting it.

**Build output**:
- Debug: `__Builds/x64/Debug`
- Release: `__Builds/x64/Release`

> [!NOTE]
> Ensure that [make](https://www.gnu.org/software/make/) is available in your environment before building.

**Usage**:
```bash
./__Builds/x64/Release/QTAC
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
