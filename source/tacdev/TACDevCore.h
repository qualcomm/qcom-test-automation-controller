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

#pragma once

#include "TACDev.h"

#include <qtac/AlpacaDevice.h>
#include <qtac/ByteArray.h>
#include <qtac/TACDriveThread.h>

#include <unordered_map>

// Qt-free core for TACDev.dll.
// Manages the table of open device handles.
class DevTACCore
{
public:
    DevTACCore() = default;
    ~DevTACCore() = default;

    // Open a device by port name (or serial number) and return its handle.
    // Returns kBadHandle on failure.
    TAC_HANDLE openHandleByDescription(const char* portName);

    // Close and remove a handle.
    TAC_RESULT closeHandle(TAC_HANDLE tacHandle);

    // Refresh the device list (all board types) and return the count.
    TAC_RESULT getDeviceCount(int* deviceCount);

    // Look up a device by handle.  Returns nullptr on bad handle.
    AlpacaDevice getAlpacaDevice(TAC_HANDLE tacHandle);

    // Returns the most-recently-enumerated device list.
    const AlpacaDevices& alpacaDevices() const { return _alpacaDevices; }

    // Last error string.
    qtac::ByteArray lastError() const { return _lastError; }
    void setLastError(const qtac::ByteArray& err) { _lastError = err; }

private:
    AlpacaDevices  _alpacaDevices;
    std::unordered_map<TAC_HANDLE, AlpacaDevice>          _openDevices;
    std::unordered_map<TAC_HANDLE, qtac::TACDriveThread*> _driveThreads;
    qtac::ByteArray _lastError;
};
