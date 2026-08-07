# QTAC Installer Guide

This guide is for anyone downloading and installing the **Qualcomm Test
Automation Controller (QTAC)** suite. It explains what the installer does,
what gets installed and where, and how to uninstall cleanly — for both
**Windows** and **Linux**.

## Table of Contents

- [What's Included](#whats-included)
- [Windows](#windows)
  - [Installing](#installing-windows)
  - [Where Files Are Installed](#where-files-are-installed-windows)
  - [Uninstalling](#uninstalling-windows)
- [Linux](#linux)
  - [Installing](#installing-linux)
  - [Where Files Are Installed](#where-files-are-installed-linux)
  - [Uninstalling](#uninstalling-linux)

---

## What's Included

The QTAC installer packages the following applications:

| Application | Type | Purpose |
| :-- | :-- | :-- |
| **Test Automation Controller (TAC)** | GUI | Main application for remotely controlling a Qualcomm device via a debug board |
| **TAC Configuration Editor** | GUI | Design/edit `.tcnf` GPIO-mapping configuration files used by TAC |
| **Device Catalog** | GUI | Program a debug board's firmware and view supported Qualcomm platforms |
| **FTDICheck** | Command-line | Verifies/installs the FTDI driver required for debug-board communication |
| **LITEProgrammer** | Command-line | Programs FTDI-based debug boards |
| **DevList** | Command-line | Lists connected Qualcomm devices/debug boards |
| **TACDump** | Command-line | Dumps device/debug-board state and logs |
| **UpdateDeviceList** | Command-line | Regenerates the device/platform configuration list |

The installer also bundles everything these applications need to run —
the Qt runtime, and the FTDI driver files — so nothing else needs to be
downloaded or installed separately.

---

## Windows

### Installing (Windows)

1. Download `QTAC-windows-<arch>.exe` (choose `x64` or `arm64` to match your
   PC) from the project's [Releases](https://github.com/qualcomm/qcom-test-automation-controller/releases)
   page or from the CI build artifacts.
2. Double-click it and accept the UAC prompt to allow the installer to run
   with administrator rights.

> [!TIP]
> If a QTAC application was already running, close it before installing so
> its files aren't locked.

### Where Files Are Installed (Windows)

| Content | Location |
| :-- | :-- |
| Applications (TAC, TACConfigEditor, DeviceCatalog, etc.) | `C:\Program Files\Qualcomm\QTAC\` |
| Device/platform configuration files | `C:\ProgramData\Qualcomm\QTAC\configurations\` |
| FTDI driver files | `C:\ProgramData\Qualcomm\QTAC\FTDI\` |
| Python & Java interface files | `C:\ProgramData\Qualcomm\QTAC\interfaces\` |
| Example automation scripts | `C:\QTAC\examples\` |


### Uninstalling (Windows)

Use **Settings → Apps → Installed apps** (or the classic
**Add or Remove Programs**) and select
**Qualcomm Test Automation Controller**, or run the uninstaller directly
from the Start Menu shortcut **Uninstall QTAC**.

> [!TIP]
> Disconnect any USB debug boards before uninstalling — the FTDI driver
> cannot always be removed while a device using it is still attached.

---

## Linux

### Installing (Linux)

Download the `.deb` package from the project's [Releases](https://github.com/qualcomm/qcom-test-automation-controller/releases)
page or CI build artifacts, then install it with:

```bash
sudo apt install ./qualcomm-qtac_<version>_<arch>.deb
```

This automatically pulls in any required system dependencies. During
installation, the required udev rule is installed and the device
configuration list is generated automatically.

### Where Files Are Installed (Linux)

| Content | Location |
| :-- | :-- |
| Applications and libraries | `/opt/qcom/QTAC/bin/`, `/opt/qcom/QTAC/lib/` |
| Documentation | `/opt/qcom/QTAC/docs/` |
| Example automation scripts | `/opt/qcom/QTAC/examples/` |
| Python interface files | `/opt/qcom/QTAC/python/` |
| Device/platform configuration files | `/var/lib/qcom/data/QTAC/configurations/` |
| USB device rule (for accessing debug boards without root) | `/etc/udev/rules.d/99-QTAC-USB.rules` |

### Uninstalling (Linux)

```bash
sudo apt remove qualcomm-qtac
```

This removes the installed udev rule and application files. Configuration
files are left in place unless you also run
`sudo apt purge qualcomm-qtac`.

---