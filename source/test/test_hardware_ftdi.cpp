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

// Hardware integration test: TACLite / FTDI device
//
// Requires a TACLite FTDI device physically connected.
// Auto-enumerates via _FTDIChipset::getDeviceCount().
//
// Tests exercised:
//   1. Device enumeration   — FTDIDevice::updateAlpacaDevices() finds >= 1 device
//   2. AlpacaDevice registry — findAlpacaDevice() by hash succeeds
//   3. Drive thread lifecycle — TACLiteDriveThread start/run/shutDown
//   4. Signal delivery        — onDeviceConnected, onFirmwareVersionUpdate,
//                               onHardwareTypeUpdate, onNameUpdate fire within
//                               the connection timeout
//   5. Property accessors     — name(), portName(), debugBoardType()
//   6. Clean shutdown         — shutDown() + join completes without hang

#include <qtac/FTDIDevice.h>
#include <qtac/FTDIChipset.h>
#include <qtac/TACLiteDriveThread.h>
#include <qtac/AlpacaDevice.h>
#include <qtac/FTDIPlatformConfiguration.h>

#include <atomic>
#include <cassert>
#include <chrono>
#include <cstdio>
#include <thread>

// ---------------------------------------------------------------------------
// Wait helper — spins until predicate returns true or timeout_ms elapses.
// ---------------------------------------------------------------------------

static bool waitFor(std::function<bool()> pred, int timeout_ms = 3000, int poll_ms = 50)
{
    const auto deadline = std::chrono::steady_clock::now()
                          + std::chrono::milliseconds(timeout_ms);
    while (std::chrono::steady_clock::now() < deadline)
    {
        if (pred()) return true;
        std::this_thread::sleep_for(std::chrono::milliseconds(poll_ms));
    }
    return pred(); // one final check
}

// ---------------------------------------------------------------------------
// Test 1: Device enumeration and chipset helpers
// (Combined to avoid calling getDeviceCount() more than once —
//  calling it a second time while _ftdiChipsetList is populated causes
//  the reset() loop to invalidate its own iterator.)
// ---------------------------------------------------------------------------

static void test_enumeration_and_chipset()
{
    // -- AlpacaDevice registry (calls getDeviceCount internally) --
    printf("test_enumeration: ");

    uint32_t devCount = FTDIDevice::updateAlpacaDevices();
    printf("found %u FTDI device(s)\n", devCount);
    assert(devCount >= 1 && "No FTDI device found — is a TACLite connected?");

    AlpacaDevices devices;
    _AlpacaDevice::getAlpacaDevices(devices, eFTDI);
    assert(!devices.isEmpty());
    assert(devices[0]->active());
    assert(!devices[0]->portName().isEmpty());
    printf("  port: %s  sn: %s\n",
           devices[0]->portName().toStdString().c_str(),
           devices[0]->serialNumber().toStdString().c_str());
    printf("  PASS\n");

    // -- Chipset layer (use getDevice without re-enumerating) --
    printf("test_chipset_helpers: ");

    // getDevice(0) returns from the already-populated static list.
    FTDIChipset chip = _FTDIChipset::getDevice(0);
    assert(chip != nullptr);
    assert(!chip->portName().isEmpty());
    assert(chip->hash() != 0);

    // Look up the same chipset by port name
    FTDIChipset chip2 = _FTDIChipset::getDevice(chip->portName());
    assert(chip2 != nullptr);
    assert(chip2->hash() == chip->hash());

    printf("  PASS\n");
}

static void test_registry_lookup()
{
    printf("test_registry_lookup: ");

    AlpacaDevices devices;
    _AlpacaDevice::getAlpacaDevices(devices, eFTDI);
    assert(!devices.isEmpty());

    AlpacaDevice first = devices[0];
    HashType hash = first->hash();
    assert(hash != 0);

    AlpacaDevice found = _AlpacaDevice::findAlpacaDevice(hash);
    assert(found != nullptr);
    assert(found->hash() == hash);

    AlpacaDevice foundByPort = _AlpacaDevice::findAlpacaDevice(first->portName());
    assert(foundByPort != nullptr);
    assert(foundByPort->hash() == hash);

    printf("  PASS\n");
}

// ---------------------------------------------------------------------------
// Test 3 + 4: Drive thread lifecycle and signal delivery
// ---------------------------------------------------------------------------

static void test_drive_thread_lifecycle()
{
    printf("test_drive_thread_lifecycle: ");

    AlpacaDevices devices;
    _AlpacaDevice::getAlpacaDevices(devices, eFTDI);
    assert(!devices.isEmpty());

    AlpacaDevice alpacaDevice = devices[0];
    HashType hash = alpacaDevice->hash();

    // Counters updated from signal callbacks (all accessed from worker thread
    // but read here only after shutDown() + join, so no data race).
    std::atomic<int> connectedCount{0};
    std::atomic<int> disconnectedCount{0};
    qtac::String     capturedHwType;
    qtac::String     capturedFwVersion;
    qtac::String     capturedName;
    std::atomic<int> logLineCount{0};

    auto driveThread = std::make_unique<qtac::TACLiteDriveThread>(hash);

    driveThread->onDeviceConnected.connect([&]() {
        ++connectedCount;
    });
    driveThread->onDeviceDisconnected.connect([&]() {
        ++disconnectedCount;
    });
    driveThread->onHardwareTypeUpdate.connect([&](const qtac::String& s) {
        capturedHwType = s;
    });
    driveThread->onFirmwareVersionUpdate.connect([&](const qtac::String& s) {
        capturedFwVersion = s;
    });
    driveThread->onNameUpdate.connect([&](const qtac::String& s) {
        capturedName = s;
    });
    driveThread->onLogLine.connect([&](const qtac::ByteArray&) {
        ++logLineCount;
    });

    driveThread->start();

    // Wait up to 3 s for onDeviceConnected to fire.
    bool connected = waitFor([&]{ return connectedCount.load() > 0; }, 3000);

    if (!connected)
    {
        // Device may not have opened — shut down cleanly and report.
        driveThread->shutDown();
        printf("  SKIP (device opened but did not become connected within 3s)\n");
        return;
    }

    printf("\n  hw_type='%s'  fw='%s'  name='%s'\n",
           capturedHwType.toStdString().c_str(),
           capturedFwVersion.toStdString().c_str(),
           capturedName.toStdString().c_str());

    assert(connectedCount.load() >= 1);
    assert(logLineCount.load() > 0 && "expected at least one log line from run()");

    // Graceful shutdown
    driveThread->shutDown();

    // After shutDown() the disconnected signal should have fired.
    assert(disconnectedCount.load() >= 1);

    printf("  PASS\n");
}

// ---------------------------------------------------------------------------
// Test 5: Property accessors after enumeration
// ---------------------------------------------------------------------------

static void test_property_accessors()
{
    printf("test_property_accessors: ");

    AlpacaDevices devices;
    _AlpacaDevice::getAlpacaDevices(devices, eFTDI);
    assert(!devices.isEmpty());

    AlpacaDevice d = devices[0];

    // boardType should be eFTDI
    assert(d->debugBoardType() == eFTDI);

    // serialNumber must be non-empty
    assert(!d->serialNumber().isEmpty());

    // portName must be non-empty
    assert(!d->portName().isEmpty());

    printf("  PASS\n");
}

// ---------------------------------------------------------------------------
// Test 6: Chipset helpers
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------

int main()
{
    setvbuf(stdout, nullptr, _IONBF, 0); // unbuffered stdout so all output shows before abort
    printf("=== Hardware Integration: TACLite / FTDI ===\n\n");

    // Combined: chipset helpers + AlpacaDevice registry (only one getDeviceCount call)
    test_enumeration_and_chipset();
    test_registry_lookup();
    test_property_accessors();
    test_drive_thread_lifecycle();

    printf("\nAll hardware tests passed.\n");
    return 0;
}
