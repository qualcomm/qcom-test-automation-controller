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

// TACDeviceBridge — Qt bridge for _AlpacaDevice.
//
// Wraps a shared_ptr<_AlpacaDevice> and forwards all qtac::Signal<> callbacks
// to Qt signals, marshalled onto the GUI thread via QMetaObject::invokeMethod
// with Qt::QueuedConnection.
//
// Usage:
//   auto device = ...; // shared_ptr<_AlpacaDevice>
//   auto bridge = new TACDeviceBridge(device, parent);
//   connect(bridge, &TACDeviceBridge::pinStateChanged, this, &MyWidget::onPinStateChanged);

#pragma once

#include <QtAdapterGlobal.h>
#include <qt_string_convert.h>

#include <qtac/AlpacaDevice.h>

#include <QByteArray>
#include <QObject>
#include <QString>

#include <memory>

class QT_ADAPTER_EXPORT TACDeviceBridge : public QObject
{
    Q_OBJECT

public:
    explicit TACDeviceBridge(std::shared_ptr<_AlpacaDevice> device,
                             QObject* parent = nullptr);
    ~TACDeviceBridge() override = default;

    std::shared_ptr<_AlpacaDevice> device() const { return _device; }

signals:
    // _AlpacaDevice signals
    void pinStateChanged(quint64 pin, bool state);
    void progress(quint8 value, int level);
    void errorEvent(const QByteArray& message);

private:
    std::shared_ptr<_AlpacaDevice> _device;
};
