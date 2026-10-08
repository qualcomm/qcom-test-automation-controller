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

// Qt-free port of STM32Device from qcommon-console.

#pragma once

#include <qtac/AlpacaDevice.h>
#include <qtac/STM32PlatformConfiguration.h>

// -----------------------------------------------------------------------
// STM32HIDDeviceInfo — enumeration result for a single HID path
// -----------------------------------------------------------------------
struct STM32HIDDeviceInfo
{
    HashType        _hash{0};
    qtac::ByteArray _devicePath;
    qtac::ByteArray _serialNumber;
};

using STM32HIDDeviceInfoList = qtac::List<STM32HIDDeviceInfo>;

// -----------------------------------------------------------------------
// STM32Device
//
// _AlpacaDevice subclass for the BugHopper V2 (STM32 HID hardware).
// HID transport is encapsulated here; drive thread calls openTransport(),
// write(), and close() to access the raw hidapi handle.
// -----------------------------------------------------------------------
class STM32Device : public _AlpacaDevice
{
public:
    STM32Device()  = default;
    ~STM32Device() override;

    // --- Static device registry ---
    static uint32_t updateAlpacaDevices();

    // --- HID enumeration ---
    static STM32HIDDeviceInfoList enumerateHIDDevices();
    static qtac::ByteArray vtpPortName(const qtac::ByteArray& serialNumber);

    // --- _AlpacaDevice ---
    bool open()          override;
    void buildMapping()  override;
    void buildCommandList();
    void setPinState(PinID pin, bool state) override;

    // --- HID transport (called from TACSTM32DriveThread) ---
    bool openTransport();
    bool write(const qtac::ByteArray& report);
    void close();

private:
    _STM32PlatformConfiguration* _stm32PlatformConfiguration{nullptr};
    qtac::ByteArray              _devicePath;
    void*                        _hidHandle{nullptr};
};
