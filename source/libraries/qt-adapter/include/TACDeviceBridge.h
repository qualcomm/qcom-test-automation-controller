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

// TACDeviceBridge — Qt bridge for _AlpacaDevice + TACDriveThread.
//
// Wraps a shared_ptr<_AlpacaDevice> and its drive thread, forwarding all
// qtac::Signal<> callbacks to Qt signals via QMetaObject::invokeMethod with
// Qt::QueuedConnection so they arrive safely on the GUI thread.
//
// Usage:
//   auto dev = _AlpacaDevice::findAlpacaDevice(portName);
//   auto* dt = new qtac::TACLiteDriveThread(dev->hash());
//   dev->setDriveThread(dt);
//   dev->open();                                // starts drive thread
//   auto* bridge = new TACDeviceBridge(dev, dt, parent);
//   connect(bridge, &TACDeviceBridge::deviceConnected, ...);
//   connect(bridge, &TACDeviceBridge::pinStateChanged, ...);

#pragma once

#include <QtAdapterGlobal.h>
#include <qt_string_convert.h>

#include <qtac/AlpacaDevice.h>
#include <qtac/AlpacaScript.h>
#include <qtac/TACDriveThread.h>

#include <QByteArray>
#include <QObject>
#include <QString>

#include <memory>

class QT_ADAPTER_EXPORT TACDeviceBridge : public QObject
{
    Q_OBJECT

public:
    // Construct with device only (no drive thread signals).
    explicit TACDeviceBridge(std::shared_ptr<_AlpacaDevice> device,
                             QObject* parent = nullptr);

    // Construct with device + drive thread (full signal set).
    TACDeviceBridge(std::shared_ptr<_AlpacaDevice> device,
                    qtac::TACDriveThread* driveThread,
                    QObject* parent = nullptr);

    ~TACDeviceBridge() override;

    void disconnectSignals();

    std::shared_ptr<_AlpacaDevice> device() const { return _device; }
    qtac::TACDriveThread* driveThread() const { return _driveThread; }

    // Execute a Quick Settings script command by name.
    void quickCommand(const QByteArray& command)
    {
        _device->quickCommand(qtac::ByteArray(command.data(), command.size()));
    }

    // Update a variable value (called from UI widgets before running a command).
    void setVariableValue(const QString& name, int value);
    void setVariableValue(const QString& name, bool value);

    // Rename the device (PSoC/PIC32CX only; no-op for FTDI).
    bool supportsRename() const;
    void renameDevice(const QString& newName);

signals:
    // _AlpacaDevice signals
    void pinStateChanged(quint64 pin, bool state);
    void progress(quint8 value, int level);
    void errorEvent(const QByteArray& message);

    // TACDriveThread signals (only emitted when a drive thread was provided)
    void deviceConnected();
    void deviceDisconnected();
    void firmwareVersionUpdated(const QString& version);
    void hardwareTypeUpdated(const QString& hwType);
    void nameUpdated(const QString& name);
    void serialNumberUpdated(const QString& sn);
    void logLine(const QByteArray& line);

private:
    void connectDeviceSignals();
    void connectDriveThreadSignals();

    std::shared_ptr<_AlpacaDevice> _device;
    qtac::TACDriveThread*          _driveThread{nullptr};

    // Connection IDs for device signals (disconnected in destructor)
    int _connPinState{0};
    int _connProgress{0};
    int _connError{0};

    // Connection IDs for drive thread signals (disconnected in destructor)
    int _connDeviceConnected{0};
    int _connDeviceDisconnected{0};
    int _connFirmwareVersion{0};
    int _connHardwareType{0};
    int _connName{0};
    int _connSerialNum{0};
    int _connLogLine{0};
};
