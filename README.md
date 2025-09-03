# Qualcomm Test Automation Controller (QTAC)

## Introduction

QTAC is a software suite that enables users to control Qualcomm devices remotely.
The device to be controlled must be attached to a Qualcomm approved debug board. The device to be tested, is
connected to a host using a USB cable.

![QTAC Software Suite](./docs/resources/qtac-sw-suite.png)

## Download pre-built binaries

Download the QTAC release from the [releases](https://github.com/qualcomm/qcom-test-automation-controller/releases)

## Build from source

Please review the following guide to build the project from source. For one time setup instructions to build from
source, please review [software install guide](#software-install-guide)

### Clone repository

Use the below command to clone the project source:

```bash
git clone https://github.com/qualcomm/qcom-test-automation-controller.git
```

### Setup third-party libraries

QTAC uses FTDI libraries to control FT4232H chip on the debug board. You can find out more about the FTDI D2XX libraries
[here](https://ftdichip.com/drivers/d2xx-drivers/). You may either download and setup the `ftd2xx` library or use
the scripts at [third-party](./third-party/) to automatically download and setup libraries. This step is required to build from source.

#### Compile QTAC for Windows

Execute the [build.bat](./build.bat) to generate the executables for `debug` as well as `release`.
Please find the output files at __Builds/x64/\<Debug>/\<Release>.

#### Compile QTAC for Linux

Execute the [build.bat](./build.sh) to generate the executables for `debug` as well as `release`.
Please find the output files at __Builds/x64/\<Debug>/\<Release>.

## Software install guide

### Install tools for development

| Category | Software | Minimum version
| :-- | :-- | :-- |
| Operating System | Windows, Debian | Windows 10 & above<br>Ubuntu 22.04 & above |
| Software development | [Visual Studio Compiler 2022](https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022) (Windows)<br>GCC (Linux) |  MSVC 2022 (Windows)<br>GCC-11, G++-11, GLIBC-2.35 (Linux) |
| Software development | [Qt Open-source](https://www.qt.io/download-qt-installer-oss) | 6.9.0 and above |

Please review the usage policies, license terms, and conditions of the above software before use.

### Configure Qt installation

QTAC requires Qt6 and MSVC2022 64-bit. Please review below custom install configuration in Qt
to optimize download time.

Required additional libraries:

1. Qt Serial Bus
2. Qt Serial Port

### Configure environment variables

With the development tools installed on your system, please set up the following environment variables:

- `QTBIN`: `C:\Qt\<version>\msvc2022_64\bin`

**On Windows**:

```cmd
setx QTBIN C:\Qt\<version>\msvc2022_64\bin
```

**On Linux**:

Start a bash terminal at the project root and ensure [make](https://www.gnu.org/software/make/) is available in the environment.
Then, execute the below commands.

```bash
export QTBIN=$/path/to/Qt/directory/<version>/gcc_64/bin
```

## Repository structure

QTAC repository has the following sub-directories:

1. `.github`: GitHub CI/CD build pipelines
2. `configurations`: Platform specific pin configurations to control devices
3. `docs`: API documentation and guide
4. `examples`: Example scripts to demonstrate the use of device-control automation APIs
5. `interfaces`: Device control APIs for C++, Python, C# and Java
6. `src`: Source files containing core libraries, CLI and UI applications
   - `applications`: CLI and UI applications
   - `libraries`: Core libraries referenced by other libraries, CLI & UI applications
7. `third-party`: Contains automation scripts to install external project dependencies

## Application dependency architecture

Applications and library in QTAC has the following overall dependency architecture
![App dependency](./docs/resources/app-dependency.drawio.png)

## Qualcomm device control using QTAC

### Hardware setup

QTAC requires you to have physical access to Qualcomm approved devices and debug boards. QTAC is device-agnostic
and supports all Qualcomm chipsets and form-factors.

The Qualcomm device may be attached directly to the debug board or through cable strip depending on the
form-factor or the guidelines outlined in the hardware manual.

If you've questions, suggestions or issues with setup, please do reach out to us on
[discord](https://discord.com/invite/qualcommdevelopernetwork).

You will have to install the [required drivers](#install-drivers) correctly. The one-time driver installation
step is taken care if you install a release package.

![QTAC Hardware Software Setup](./docs/resources/qtac-setup-overview.drawio.png)

### Install drivers

The one-time driver installation step is taken care if you install a release package. If you choose
to [build from source](#build-from-source), configure below drivers:

1. [FTDI D2XX drivers](https://ftdichip.com/drivers/d2xx-drivers/): to interact with the debug board
2. [Qualcomm USB Drivers](https://softwarecenter.qualcomm.com/catalog/item/QUD): to view device
   status (Emergency Download Mode, USB Diagonistics Mode, etc)

### Optional software

QTAC allows you to view the streaming device logs as you transition the device between different
states. The debug logs are streamed over USB serial interface(s).

To view these logs, you may install [Putty](https://www.putty.org/) or a similar software. QTAC
does not depend or use this software.

## Bug & Vulnerability reporting

Please review the [SECURITY.md](./.github/SECURITY.md) before reporting vulnerabilities with the project

## Contributor's License Agreement

Please review the Qualcomm product [license](./LICENSE), [code of conduct](./CODE-OF-CONDUCT.md) & terms
and conditions before contributing.

## Using Qt Creator for development

If you're interested in compiling thr project using Qt Creator, please refer the
[01-Build-Using-Qt-Creator](./docs/getting-started/01-Build-Using-Qt-Creator.md) guide.
