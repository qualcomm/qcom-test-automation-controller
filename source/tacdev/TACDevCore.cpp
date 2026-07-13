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

#include "TACDevCore.h"

#include <qtac/FTDIDevice.h>
#include <qtac/PSOCDevice.h>
#include <qtac/PIC32CXDevice.h>
#include <qtac/TACLiteDriveThread.h>
#include <qtac/TACPSOCDriveThread.h>
#include <qtac/TACPIC32CXDriveThread.h>

// ---------------------------------------------------------------------------

TAC_RESULT DevTACCore::getDeviceCount(int* deviceCount)
{
    *deviceCount = 0;

    FTDIDevice::updateAlpacaDevices();
    PSOCDevice::updateAlpacaDevices();
    PIC32CXDevice::updateAlpacaDevices();

    _AlpacaDevice::getAlpacaDevices(_alpacaDevices);
    *deviceCount = static_cast<int>(_alpacaDevices.size());

    return NO_TAC_ERROR;
}

TAC_HANDLE DevTACCore::openHandleByDescription(const char* portName)
{
    // Return existing handle if device is already open.
    for (const auto& [handle, dev] : _openDevices)
    {
        if (dev->portName() == portName)
            return handle;
        if (dev->serialNumber().contains(portName))
            return handle;
    }

    // Ensure device list is fresh for all board types.
    FTDIDevice::updateAlpacaDevices();
    PSOCDevice::updateAlpacaDevices();
    PIC32CXDevice::updateAlpacaDevices();

    AlpacaDevice dev = _AlpacaDevice::findAlpacaDevice(
        qtac::ByteArray(portName));
    if (!dev)
    {
        setLastError(qtac::ByteArray(portName) + " not found.");
        return kBadHandle;
    }

    // Create and inject the concrete drive thread.
    qtac::TACDriveThread* dt = nullptr;
    switch (dev->debugBoardType())
    {
    case ePSOC:
        dt = new qtac::TACPSOCDriveThread(dev->hash());
        break;
    case ePIC32CXAuto:
        dt = new qtac::TACPIC32CXDriveThread(dev->hash());
        break;
    case eFTDI:
    default:
        dt = new qtac::TACLiteDriveThread(dev->hash());
        break;
    }
    dev->setDriveThread(dt);

    // Subscribe to error events.
    dev->onError.connect([this](const qtac::ByteArray& msg) {
        setLastError(msg);
    });

    if (!dev->open())
    {
        setLastError(dev->getLastError().isEmpty()
            ? qtac::ByteArray(portName) + " can't be opened."
            : dev->getLastError());
        dev->setDriveThread(nullptr);
        delete dt;
        dev->close();
        return kBadHandle;
    }

    TAC_HANDLE handle = dev->hash();
    _openDevices[handle] = dev;
    _driveThreads[handle] = dt;
    return handle;
}

TAC_RESULT DevTACCore::closeHandle(TAC_HANDLE tacHandle)
{
    auto it = _openDevices.find(tacHandle);
    if (it == _openDevices.end())
        return TACDEV_BAD_TAC_HANDLE;

    AlpacaDevice dev = it->second;
    dev->close();

    auto dtIt = _driveThreads.find(tacHandle);
    if (dtIt != _driveThreads.end())
    {
        qtac::TACDriveThread* dt = dtIt->second;
        if (dt)
        {
            dt->shutDown();
            delete dt;
        }
        dev->setDriveThread(nullptr);
        _driveThreads.erase(dtIt);
    }

    _openDevices.erase(it);
    return NO_TAC_ERROR;
}

AlpacaDevice DevTACCore::getAlpacaDevice(TAC_HANDLE tacHandle)
{
    auto it = _openDevices.find(tacHandle);
    if (it != _openDevices.end())
        return it->second;

    setLastError("Bad TAC Handle");
    return AlpacaDevice(nullptr);
}
