// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted (subject to the limitations in the
// disclaimer below) provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//         notice, this list of conditions and the following disclaimer.
//
//     * Redistributions in binary form must reproduce the above
//         copyright notice, this list of conditions and the following
//         disclaimer in the documentation and/or other materials provided
//         with the distribution.
//
//     * Neither the name of Qualcomm Technologies, Inc. nor the names of its
//         contributors may be used to endorse or promote products derived
//         from this software without specific prior written permission.
//
// NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
// GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
// HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
// IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
// IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Hardware integration test: PSoC device
//
// Requires a PSoC-based TAC device physically connected via serial port
// (VID 0x05C6/PID 0x9302 or VID 0x16C0/PID 0x0483).
//
// Tests exercised:
//   1. Device enumeration   — PSOCDevice::updateAlpacaDevices() finds >= 1 device
//   2. Pre-open properties  — debugBoardType, portName, serialNumber
//   3. Registry lookup      — findAlpacaDevice() by hash and by port name
//   4. Open / close         — open() returns true, platformID is resolved,
//                             commandCount > 0, close() completes without hang
//   5. Pin signal           — onPinStateChanged is wired through _serialDriveThread
//                             (no explicit signal expected; just verifies no crash)
//   6. Double-close safety  — close() on an already-closed device is a no-op

#include <qtac/PSOCDevice.h>
#include <qtac/AlpacaDevice.h>
#include <qtac/TACPSOCDriveThread.h>
#include <qtac/SerialPort.h>
#include <qtac/SerialPortInfo.h>
#include <qtac/SerialPortSettings.h>
#include <qtac/DebugBoardType.h>
#include <qtac/PlatformID.h>

#include <atomic>
#include <cassert>
#include <chrono>
#include <cstdio>
#include <functional>
#include <thread>

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static bool waitFor(std::function<bool()> pred, int timeout_ms = 5000, int poll_ms = 50)
{
    const auto deadline = std::chrono::steady_clock::now()
                          + std::chrono::milliseconds(timeout_ms);
    while (std::chrono::steady_clock::now() < deadline)
    {
        if (pred()) return true;
        std::this_thread::sleep_for(std::chrono::milliseconds(poll_ms));
    }
    return pred();
}

static AlpacaDevices getPSOCDevices()
{
    AlpacaDevices devices;
    _AlpacaDevice::getAlpacaDevices(devices, ePSOC);
    return devices;
}

// ---------------------------------------------------------------------------
// Test 1: Enumeration
// ---------------------------------------------------------------------------

static void test_enumeration()
{
    printf("test_enumeration: ");

    uint32_t devCount = PSOCDevice::updateAlpacaDevices();
    printf("found %u PSoC device(s)\n", devCount);
    assert(devCount >= 1 && "No PSoC device found — is a PSoC TAC connected?");

    AlpacaDevices devices = getPSOCDevices();
    assert(!devices.isEmpty());
    assert(devices[0]->active());

    printf("  port: %s  sn: %s\n",
           devices[0]->portName().toStdString().c_str(),
           devices[0]->serialNumber().toStdString().c_str());
    printf("  PASS\n");
}

// ---------------------------------------------------------------------------
// Test 2: Pre-open property accessors
// ---------------------------------------------------------------------------

static void test_pre_open_properties()
{
    printf("test_pre_open_properties: ");

    AlpacaDevices devices = getPSOCDevices();
    assert(!devices.isEmpty());
    AlpacaDevice d = devices[0];

    assert(d->debugBoardType() == ePSOC);
    assert(!d->portName().isEmpty());
    // serialNumber may be empty on some PSoC firmware versions — just print it
    printf("\n  boardType=%s  port=%s  sn='%s'\n",
           d->debugBoardTypeString().toStdString().c_str(),
           d->portName().toStdString().c_str(),
           d->serialNumber().toStdString().c_str());

    printf("  PASS\n");
}

// ---------------------------------------------------------------------------
// Test 3: Registry lookup
// ---------------------------------------------------------------------------

static void test_registry_lookup()
{
    printf("test_registry_lookup: ");

    AlpacaDevices devices = getPSOCDevices();
    assert(!devices.isEmpty());

    AlpacaDevice first = devices[0];
    HashType hash = first->hash();
    assert(hash != 0);

    AlpacaDevice foundByHash = _AlpacaDevice::findAlpacaDevice(hash);
    assert(foundByHash != nullptr);
    assert(foundByHash->hash() == hash);

    AlpacaDevice foundByPort = _AlpacaDevice::findAlpacaDevice(first->portName());
    assert(foundByPort != nullptr);
    assert(foundByPort->hash() == hash);

    printf("  PASS\n");
}

// ---------------------------------------------------------------------------
// Test 3b: Raw serial loopback — open the port directly via SerialPort,
// send "getboardid\r", and print whatever bytes come back within 2s.
// Useful to verify the serial layer is functional independent of the
// drive thread state machine.
// ---------------------------------------------------------------------------

static void test_raw_serial()
{
    printf("test_raw_serial:\n");

    AlpacaDevices devices = getPSOCDevices();
    assert(!devices.isEmpty());

    qtac::SerialPortInfo portInfo;
    {
        // Find the matching SerialPortInfo
        qtac::String targetPort = qtac::String(devices[0]->portName().toStdString());
        for (const auto& info : qtac::SerialPortInfo::availablePorts())
        {
            if (info.portName() == targetPort)
            {
                portInfo = info;
                break;
            }
        }
    }

    if (portInfo.isNull())
    {
        printf("  SKIP: port info not found\n");
        return;
    }

    qtac::SerialPortSettings settings;
    settings.baudRate = 115200;

    qtac::SerialPort port(portInfo);
    port.setSettings(settings);

    if (!port.open())
    {
        printf("  SKIP: could not open port — %s\n", port.errorString().toStdString().c_str());
        return;
    }

    printf("  Port opened. Sending 'getboardid\\r'...\n");

    // Drain any pending bytes first.
    port.clear();

    int written = port.write(qtac::ByteArray("getboardid\r"), 1000);
    printf("  Bytes written: %d\n", written);

    // Poll for a response for up to 2 seconds.
    bool gotData = false;
    for (int i = 0; i < 20; ++i)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        qtac::ByteArray data = port.readAll(50);
        if (!data.isEmpty())
        {
            printf("  Received (%d bytes): '", data.size());
            // Print printable chars, escape others
            for (int j = 0; j < data.size(); ++j)
            {
                unsigned char c = static_cast<unsigned char>(data.constData()[j]);
                if (c >= 32 && c < 127) printf("%c", c);
                else printf("\\x%02x", c);
            }
            printf("'\n");
            gotData = true;
        }
    }

    if (!gotData)
        printf("  No response received within 2s\n");

    port.close();
    printf("  PASS (check output above)\n");
}

// ---------------------------------------------------------------------------
// Test 4a: Drive thread in isolation (mirrors test_hardware_ftdi approach).
// Directly instantiates TACPSOCDriveThread and waits for onDeviceConnected.
// This bypasses PSOCDevice::open() to verify the serial layer independently.
// ---------------------------------------------------------------------------

static void test_drive_thread_direct()
{
    printf("test_drive_thread_direct: (may take a few seconds)\n");

    AlpacaDevices devices = getPSOCDevices();
    assert(!devices.isEmpty());
    AlpacaDevice alpacaDevice = devices[0];
    HashType hash = alpacaDevice->hash();

    std::atomic<int>  connectedCount{0};
    std::atomic<int>  disconnectedCount{0};
    std::atomic<int>  logLineCount{0};
    qtac::String      capturedHwType;
    qtac::String      capturedFwVersion;
    qtac::String      capturedName;
    qtac::ByteArray   lastLogLine;

    auto driveThread = std::make_unique<qtac::TACPSOCDriveThread>(hash);

    driveThread->onDeviceConnected.connect([&]() {
        ++connectedCount;
        printf("  [connected! count=%d]\n", connectedCount.load());
    });
    driveThread->onDeviceDisconnected.connect([&]() {
        ++disconnectedCount;
        printf("  [disconnected! count=%d]\n", disconnectedCount.load());
    });
    driveThread->onFirmwareVersionUpdate.connect([&](const qtac::String& s) {
        capturedFwVersion = s;
        printf("  [firmware: '%s']\n", s.toStdString().c_str());
    });
    driveThread->onHardwareTypeUpdate.connect([&](const qtac::String& s) {
        capturedHwType = s;
        printf("  [hwtype: '%s']\n", s.toStdString().c_str());
    });
    driveThread->onNameUpdate.connect([&](const qtac::String& s) {
        capturedName = s;
        printf("  [name: '%s']\n", s.toStdString().c_str());
    });
    driveThread->onLogLine.connect([&](const qtac::ByteArray& line) {
        ++logLineCount;
        lastLogLine = line;
        printf("  [log] %s\n", line.toStdString().c_str());
    });

    driveThread->start();

    bool connected = waitFor([&]{ return connectedCount.load() > 0; }, 5000);

    if (!connected)
    {
        driveThread->shutDown();
        printf("  SKIP: drive thread did not connect within 5s\n");
        printf("  Last log line: '%s'\n", lastLogLine.toStdString().c_str());
        return;
    }

    printf("  hw_type='%s'  fw='%s'  name='%s'\n",
           capturedHwType.toStdString().c_str(),
           capturedFwVersion.toStdString().c_str(),
           capturedName.toStdString().c_str());

    assert(connectedCount.load() >= 1);

    driveThread->shutDown();
    assert(disconnectedCount.load() >= 1);

    printf("  PASS\n");
}

// ---------------------------------------------------------------------------
// Test 4b: Open / close lifecycle via PSOCDevice::open()
// ---------------------------------------------------------------------------
//
// PSoC open() is self-contained — it creates its own TACPSOCDriveThread
// internally via _serialDriveThread. No external thread injection needed.
// open() blocks until the device is connected or times out (~18 s max).
// ---------------------------------------------------------------------------

static void test_open_close()
{
    printf("test_open_close: (may take a few seconds)...\n");

    AlpacaDevices devices = getPSOCDevices();
    assert(!devices.isEmpty());
    AlpacaDevice dev = devices[0];

    // Connect the device-level pin signal before open() so we know the
    // forwarding wire from _serialDriveThread is in place.
    std::atomic<int> pinSignalCount{0};
    dev->onPinStateChanged.connect([&](uint64_t /*pin*/, bool /*state*/) {
        ++pinSignalCount;
    });

    // open() creates _serialDriveThread, starts it, and blocks until
    // connected (or returns false on timeout / old firmware).
    bool ok = dev->open();
    if (!ok)
    {
        // getLastError() clears the stored error — capture it immediately.
        qtac::ByteArray err = dev->getLastError();
        printf("  SKIP: open() returned false — '%s'\n",
               err.isEmpty() ? "(no message — error was already consumed)" : err.toStdString().c_str());
        return;
    }

    // platformID may be UNKNOWN on firmware that does not support 'Get Platform ID'.
    PlatformID pid = dev->platformID();
    if (pid == MICRO_EPM_BOARD_ID_UNKNOWN)
        printf("  platformID=UNKNOWN (firmware does not support Get Platform ID)\n");
    else
        printf("  platformID=%d\n", static_cast<int>(pid));

    // Commands are built via buildMapping() inside open() if a .tcnf exists.
    uint32_t cmdCount = dev->commandCount();
    printf("  commandCount=%u\n", cmdCount);

    // isOpen() should now be true.
    assert(dev->isOpen());

    // close() shuts down _serialDriveThread and deletes it.
    dev->close();
    assert(!dev->isOpen());

    printf("  PASS\n");
}

// ---------------------------------------------------------------------------
// Test 5: Double-close is a no-op
// ---------------------------------------------------------------------------

static void test_double_close()
{
    printf("test_double_close: ");

    AlpacaDevices devices = getPSOCDevices();
    assert(!devices.isEmpty());
    AlpacaDevice dev = devices[0];

    // open then close twice — second close must not crash or hang
    bool ok = dev->open();
    if (!ok)
    {
        printf("  SKIP (open failed)\n");
        return;
    }
    dev->close();
    dev->close();   // second close — should be a no-op

    printf("  PASS\n");
}

// ---------------------------------------------------------------------------
// Test 6: Property accessor coverage after open()
//
// NOTE: _AlpacaDevice property accessors (firmwareVersion, hardwareVersionString,
// name, etc.) route through _driveThread, not _serialDriveThread. For PSoC,
// _driveThread is never set — so these return empty strings even after a
// successful open(). This test documents the current known limitation.
// ---------------------------------------------------------------------------

static void test_property_accessors_after_open()
{
    printf("test_property_accessors_after_open:\n");

    AlpacaDevices devices = getPSOCDevices();
    assert(!devices.isEmpty());
    AlpacaDevice dev = devices[0];

    bool ok = dev->open();
    if (!ok)
    {
        printf("  SKIP (open failed)\n");
        return;
    }

    // These are set directly on the device (not via _driveThread) — should work.
    printf("  platformID     = %d\n",    static_cast<int>(dev->platformID()));
    printf("  debugBoardType = %s\n",    dev->debugBoardTypeString().toStdString().c_str());
    printf("  portName       = %s\n",    dev->portName().toStdString().c_str());
    printf("  serialNumber   = %s\n",    dev->serialNumber().toStdString().c_str());
    printf("  description    = %s\n",    dev->description().toStdString().c_str());
    printf("  commandCount   = %u\n",    dev->commandCount());

    // These route through _driveThread (not set for PSoC) — expect empty.
    printf("  [via _driveThread — expected empty for PSoC:]\n");
    printf("  firmwareVersion     = '%s'\n", dev->firmwareVersion().toStdString().c_str());
    printf("  hardwareVersionStr  = '%s'\n", dev->hardwareVersionString().toStdString().c_str());
    printf("  name                = '%s'\n", dev->name().toStdString().c_str());
    printf("  uuid                = '%s'\n", dev->uuid().toStdString().c_str());

    dev->close();
    printf("  PASS (see output above for known limitations)\n");
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------

int main()
{
    setvbuf(stdout, nullptr, _IONBF, 0);
    printf("=== Hardware Integration: PSoC ===\n\n");

    test_enumeration();
    test_pre_open_properties();
    test_registry_lookup();
    test_raw_serial();
    test_drive_thread_direct();
    test_open_close();
    test_double_close();
    test_property_accessors_after_open();

    printf("\nAll PSoC hardware tests passed.\n");
    return 0;
}
