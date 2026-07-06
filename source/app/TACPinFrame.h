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

#include <TACDeviceBridge.h>

#include <qtac/PinEntry.h>
#include <qtac/CommandGroup.h>

#include <QWidget>
#include <QMap>
#include <QPushButton>
#include <QLabel>

// ---------------------------------------------------------------------------
// TACPinFrame — pin button panel.
//
// Reads the active Pins from the device's platform configuration and builds
// a grid of toggle buttons (one per pin), grouped by command group and tab.
// Pin-state updates from TACDeviceBridge are reflected by colouring the
// button.
//
// This replaces ui-common/TACFrame and ui-common/PinLED for the parallel GUI.
// ---------------------------------------------------------------------------
class TACPinFrame : public QWidget
{
    Q_OBJECT

public:
    explicit TACPinFrame(QWidget* parent = nullptr);
    ~TACPinFrame() override = default;

    void setDevice(TACDeviceBridge* bridge);
    void clearDevice();

    // Called by TACWindow when pinStateChanged arrives.
    void updatePinState(quint64 pin, bool state);

private slots:
    void onPinButtonToggled(bool checked);

private:
    void buildPins(const Pins& pins);
    void clearPins();

    TACDeviceBridge*             _bridge{nullptr};

    // pin hash → toggle button
    QMap<quint64, QPushButton*>  _pinButtons;
};
