# Build QTAC using Qt Creator

The [README](../../README.md) outlines the steps required to build QTAC using command line interface.
To understand and modify the Qt UI components, you will need to get Qt Creator. If you've already
installed Qt, you may execute the **Qt Maintenance Tool** to download the additional software Qt
Creator. If you're performing a fresh install, use the [Qt Online Installer](https://www.qt.io/download-open-source).

## Configure Qt installation
QTAC requires Qt 6.9+. Install the component matching your platform and target architecture:

**Windows**
- **x64**: `MSVC 2022 64-bit`
- **ARM64**: `MSVC 2022 ARM64`

**Linux**
- **x86_64**: `GCC 64-bit`

Please review below custom install configuration in Qt to optimize download time.

![Qt installation](../resources/qt-install-config.png)


Required additional libraries:
1. Qt Multimedia
2. Qt Serial Port

![Qt additional dependencies](../resources/qtac-additional-dependencies.png)

### Setup CMake and Ninja
QTAC requires CMake and Ninja for building from source. Both can be installed via the Qt installer, separately, or via your system package manager (`sudo apt install ninja-build cmake` on Linux).
Review the following screenshot to setup CMake on your system.

![Qt CMake dependencies](../resources/qtac-cmake-install.png)

## Clone repository
Use the below command to clone the project source:
```bash
git clone https://github.com/qualcomm/qcom-test-automation-controller.git
```

## Configure Qt 6.9+
Open the session. The **Project** tab on the left pane will be inactive. Create a
[Sample Qt project](https://doc.qt.io/qtcreator/creator-project-creating.html) to see if Qt is set up properly.

The **Project** tab now becomes active. Review the Qt kit configuration. Make sure no stray paths or kits are present
as they can lead to erroneous libraries and applications.

For Windows x64, ensure a kit is configured with:
- **Compiler**: MSVC 2022 x64 (`cl.exe` from the x64 toolchain)
- **Qt version**: the `msvc2022_64` Qt installation

For ARM64 (Windows), ensure a kit is configured with:
- **Compiler**: MSVC 2022 ARM64 (`cl.exe` from the ARM64 toolchain)
- **Qt version**: the `msvc2022_arm64` Qt installation

For Linux, ensure a kit is configured with:
- **Compiler**: GCC (g++ from the system toolchain, minimum GCC 11)
- **Qt version**: the `gcc_64` Qt installation

> [!NOTE]
> On Linux, install the udev rules before running the application:
> ```bash
> sudo cp udev-rules/99-QTAC-USB.rules /etc/udev/rules.d/
> sudo udevadm control --reload
> ```

The numbers on the image refers to some of the areas you need to review before building the project.

![Qt Creator Kits](../resources/qt-creator-configuration.png)

## Setup third-party libraries
QTAC uses FTDI libraries to control FT4232H chip on the debug board. You can find out more about the FTDI D2XX libraries
[here](https://ftdichip.com/drivers/d2xx-drivers/). The CMake scripts download the correct library automatically during configuration.

If the automatic download fails (e.g. due to network restrictions), download the archive manually and place it in the `third-party/` directory before opening the project in Qt Creator:

| Platform | Archive | URL |
| :-- | :-- | :-- |
| **Windows x64** | `CDM-v2.12.36.4-WHQL-Certified.zip` | https://web.archive.org/web/20250820134143/https://ftdichip.com/wp-content/uploads/2023/09/CDM-v2.12.36.4-WHQL-Certified.zip |
| **Windows ARM64** | `CDM-v2.12.36.20-for-ARM64-WHQL-Certified.zip` | https://web.archive.org/web/20250821211500/https://ftdichip.com/wp-content/uploads/2025/03/CDM-v2.12.36.20-for-ARM64-WHQL-Certified.zip |
| **Linux x86_64** | `libftd2xx-linux-x86_64-1.4.33.tgz` | https://web.archive.org/web/20250822044524/https://ftdichip.com/wp-content/uploads/2025/03/libftd2xx-linux-x86_64-1.4.33.tgz |

## Open project
Now you're set to build the project using Qt Creator.

Open the [CMakeLists.txt](../../CMakeLists.txt) inside Qt Creator. This sets up the project structure for you in the editor.
You do not need to manually configure Qt dependencies or open sub-projects.

![Load Qt project](../resources/qt-creator-open-project.png)

## Load project
![Load Qt project](../resources/qt-creator-open-project-cmake.png)

## Configure project
![Configure Qt project](../resources/qt-creator-configure-project.png)

## Build project
Right-click on the project you wish to build, debug, run to compile, debug and execute respectively.

![Load Qt project](../resources/qt-creator-rebuild-project.png)

## Further reading
To learn more about Qt, please refer the following links:
1. Qt installation: https://doc.qt.io/qt-6/qt-online-installation.html
2. Qt Creator: https://doc.qt.io/qtcreator/index.html
