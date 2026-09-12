# QTAC on Linux

## Introduction
QTAC on Linux is a combination of software and circuitry which integrates debug, power measurement and
test automation capabilities into a single board. The device to be tested is connected to the host using a USB cable.

## Supported Linux distributions
QTAC on Linux is supported for Ubuntu 22.04 and above. Latest version of QTAC may not correctly
for older versions of Linux.

## Install using QPM
QTAC on Linux can be downloaded using [Qualcomm Package Manager](https://qpm.qualcomm.com) or installed
using the qpm-cli by issuing the command `qpm-cli --install QTAC`

> It is important to execute the `postinstall.sh` script present at ~/QTAC/postinstall.sh. The postinstall
script contains ruels that help the host machine recognize the connected TAC devices.

> After executing the `postinstall.sh` script, the rules get only applied on the devices yet to be connected to the setup.
If the TAC device you're using is already connected to the machine, you will have to physically disconnect and reconnect it.
The other option is to reboot the machine.

If you face issues while executing the postinstall script due to terminal choices, try displaying the contents of the file
using `cat postinstall.sh` and manually executing the commands on the terminal. In upcoming versions of QTAC, we will
look into a way to address the problem with varying terminals.

## Necessary libraries and versions
QTAC requires the following libraries on the Linux machine to work optimally. If applications shipped with
QTAC fails to start, use the install command column to install the required Linux packages. It is always a
good idea to execute `sudo apt update` and `sudo apt upgrade -y` before trying to install the packages.

| Name | Library | Notes | Configuration options | Install command |
| ---- | ----| ---- | ---- | ---- |
| xcb-cursor0 | libxcb-cursor0 | Utility library for XCB for cursor | auto-detected | `sudo apt install -y libxcb-cursor0` |
| libpcre2-16-0 | libpcre2-16-0 | New perl compatible regular expression library | auto-detected | `sudo apt install -y libpcre2-16-0` |
| libusb-0.1-4 | libusb-0.1-4 | Userspace USB programming library | auto-detected | `sudo apt install -y libusb-0.1-4` |
| libxkbcommon-x11 | libxkbcommon-x11-0 | Library to create keymaps with the XKB X11 protocol | auto-detected | `sudo apt install -y libxkbcommon-x11-0` |
| libxcb-xkb | libxcb-xkb1 | X C Binding, XKEYBOARD extension | auto-detected | `sudo apt install -y libxcb-xkb1` |
| libxcb-icccm | libxcb-icccm4 | Utility libraries for X C Binding -- icccm | auto-detected | `sudo apt install -y libxcb-icccm4` |
| libxcb-shape0 | libxcb-shape0 | The shape extension for the X C Binding | auto-detected | `sudo apt -y install libxcb-shape0` |
| libxcb-keysyms | libxcb-keysyms1 | Utility libraries for X C Binding -- keysyms | auto-detected | `sudo apt install -y libxcb-keysyms1` |
| libgl1 | libgl1 | Vendor neutral GL dispatch library -- legacy GL support | auto-detected | `sudo apt -y install libgl1` |
| libegl-dev | libegl-dev | Vendor-neutral dispatch layer for arbitrating OpenGL API | auto-detected | `sudo apt -y install libegl-dev` |
| libxcb-xinerama0 | libxcb-xinerama0 | The xinerama extension for the X C Binding | auto-detected | `sudo apt-get install --reinstall libxcb-xinerama0` |
| gstreamer1.0 | gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-libav | Media playback libraries | auto-detected | `sudo apt install gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-libav` |

## Application install location

Once installed, the application binaries are present at **/opt/qcom/QTAC/bin**. The offline documentation is
present at /opt/qcom/QTAC/docs. The python APIs are present at **/opt/qcom/QTAC/python** and
example programs are in **/opt/qcom/QTAC/examples**.

## Examples

Refer the documentation for example tutorials on:
- TACDev: Python library for QTAC automation control
- EPMDev: Python library for QTAC power measurements. You will need a PSOC debug board
- UDASDev: Python library for QTAC interpreting and parsing EPM data. A physical connection to the debug board
is not required in order to work with this library

## Troubleshooting
In case you are facing issues with the debug board and QTAC, you can perform the following basic triage to decide
if you're running into a setup issue. You can always report the issue to us if you find the triage steps complicated :)

1. The debug board is not connected to the host
2. The debug board firmware is old
3. Multiple udev rules that are conflicting
4. The logged-in linux user does not have access to the dialout group
5. Application dependencies are missing on the linux installation
6. The debug board is in use by another process
7. The debug board is damaged and / or malfunctioning

## FAQ
**Q. Why are we not supporting older flavours of Ubuntu 22.04?**

A. Older versions of Ubuntu do not have the features needed for modern debug board.

**Q. Why is the versioning of QTAC on Linux not consistent / incremental?**

A. The linux release is always performed after the Windows release. We try to keep the version consistency for Linux. When we
cannot make a release in time, we match the versioning with that for Windows to provide users with the insight of what features
they can expect with the Linux release.

**Q. I have connected a debug board to my setup but I cannot connect to the TAC device using TAC. Why?**

A. Make sure that the physical connection from the host to the debug board exists. You can verify the presence of the connected
device by executing `lsusb`.

If you have a PSOC debug board, you should see two connected usb devices.
1. `Qualcomm, Inc. Embedded Power Measurement (EPM) device`
2. `Future Technology Devices International, Ltd FT4232H Quad HS USB-UART/FIFO IC`

If you have connected the **ALPACA-LITE** debug board, you should see the following FTDI device.
1. `Future Technology Devices International, Ltd FT4232H Quad HS USB-UART/FIFO IC`

If you still cannot connect to the TAC device, please make sure you do not have conflicting / unknown USB rules at
`/etc/udev/rules.d`. Presence of conflicting rules can hinder proper access to the TAC device. QTAC will only
add the following udev rule:
1. 99-FTDI-USB.rules

If you do not find the above rules in the directory, please execute `postinstall.sh` from `/opt/qcom/QTAC/util`.

If you still cannot connect to the TAC device through TAC, make sure the user is part of the  **dialout** group. You can check
group access for the user using `groups _<usename>_`

To add a username to the **dialout** group for access to serial devices, execute `sudo adduser _<username>_ dialout`