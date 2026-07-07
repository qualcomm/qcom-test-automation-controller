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

#define QT_ADAPTER_LIBRARY
#include "TACDeviceBridge.h"

#include <QMetaObject>

// ---------------------------------------------------------------------------

TACDeviceBridge::TACDeviceBridge(std::shared_ptr<_AlpacaDevice> device,
                                 QObject* parent)
    : QObject(parent)
    , _device(std::move(device))
{
    connectDeviceSignals();
}

TACDeviceBridge::TACDeviceBridge(std::shared_ptr<_AlpacaDevice> device,
                                 qtac::TACDriveThread* driveThread,
                                 QObject* parent)
    : QObject(parent)
    , _device(std::move(device))
    , _driveThread(driveThread)
{
    connectDeviceSignals();
    if (_driveThread)
        connectDriveThreadSignals();
}

// ---------------------------------------------------------------------------

void TACDeviceBridge::connectDeviceSignals()
{
    _device->onPinStateChanged.connect([this](uint64_t pin, bool state) {
        QMetaObject::invokeMethod(this, [this, pin, state]() {
            emit pinStateChanged(static_cast<quint64>(pin), state);
        }, Qt::QueuedConnection);
    });

    _device->onProgress.connect([this](uint8_t value, int level) {
        QMetaObject::invokeMethod(this, [this, value, level]() {
            emit progress(static_cast<quint8>(value), level);
        }, Qt::QueuedConnection);
    });

    _device->onError.connect([this](const qtac::ByteArray& message) {
        QByteArray msg = QtAdapter::toQByteArray(message);
        QMetaObject::invokeMethod(this, [this, msg]() {
            emit errorEvent(msg);
        }, Qt::QueuedConnection);
    });
}

void TACDeviceBridge::connectDriveThreadSignals()
{
    _driveThread->onDeviceConnected.connect([this]() {
        QMetaObject::invokeMethod(this, [this]() {
            emit deviceConnected();
        }, Qt::QueuedConnection);
    });

    _driveThread->onDeviceDisconnected.connect([this]() {
        QMetaObject::invokeMethod(this, [this]() {
            emit deviceDisconnected();
        }, Qt::QueuedConnection);
    });

    _driveThread->onFirmwareVersionUpdate.connect([this](const qtac::String& v) {
        QString s = QtAdapter::toQString(v);
        QMetaObject::invokeMethod(this, [this, s]() {
            emit firmwareVersionUpdated(s);
        }, Qt::QueuedConnection);
    });

    _driveThread->onHardwareTypeUpdate.connect([this](const qtac::String& v) {
        QString s = QtAdapter::toQString(v);
        QMetaObject::invokeMethod(this, [this, s]() {
            emit hardwareTypeUpdated(s);
        }, Qt::QueuedConnection);
    });

    _driveThread->onNameUpdate.connect([this](const qtac::String& v) {
        QString s = QtAdapter::toQString(v);
        QMetaObject::invokeMethod(this, [this, s]() {
            emit nameUpdated(s);
        }, Qt::QueuedConnection);
    });

    _driveThread->onSerialNumUpdate.connect([this](const qtac::String& v) {
        QString s = QtAdapter::toQString(v);
        QMetaObject::invokeMethod(this, [this, s]() {
            emit serialNumberUpdated(s);
        }, Qt::QueuedConnection);
    });

    _driveThread->onLogLine.connect([this](const qtac::ByteArray& line) {
        QByteArray ba = QtAdapter::toQByteArray(line);
        QMetaObject::invokeMethod(this, [this, ba]() {
            emit logLine(ba);
        }, Qt::QueuedConnection);
    });
}
