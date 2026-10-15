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

// Author: Michael Simpson
// Qt-free reimplementation of TACLiteCommand from qcommon-console.

#pragma once

#include <qtac/ByteArray.h>
#include <qtac/FramePackage.h>
#include <qtac/SendInterface.h>

#include <cstdint>

namespace qtac {

class ReceiveInterface;

// -----------------------------------------------------------------------
// TACLiteCommand
//
// RAII command builder for the FTDI (TAC Lite) drive thread.
// On destruction the destructor automatically enqueues an EndTransaction.
// -----------------------------------------------------------------------
class TACLiteCommand
{
public:
    TACLiteCommand(SendInterface* sender, ReceiveInterface* receiver);
    ~TACLiteCommand();

    TACLiteCommand(const TACLiteCommand&)            = delete;
    TACLiteCommand& operator=(const TACLiteCommand&) = delete;

    void version();
    void name();
    void uuid();

    void setPinState(uint16_t pin, bool state);

    void battery(bool state);
    void usb0(bool state);
    void usb1(bool state);

    void externalPowerControl(bool state);

    void powerKey(bool state);
    void volumeUp(bool state);
    void volumeDown(bool state);

    void setName(const qtac::ByteArray& newName);

    void getResetCount();
    void clearResetCount();

    void i2CReadRegister(uint32_t addr, uint32_t reg);
    void i2CWriteRegister(uint32_t addr, uint32_t reg, uint32_t data);

    void send(const qtac::ByteArray& command, const Arguments& arguments,
              bool console, bool store = true);
    void addDelay(uint32_t delayInMilliSeconds);
    void addLogComment(const qtac::ByteArray& comment);
    void addEndTransaction();

    // Switches
    void disconnectUIM1Button(bool state);
    void disconnectUIM2Button(bool state);
    void forcePSHoldHigh(bool state);
    void disconnectSDCard(bool state);
    void primaryEDL(bool state);
    void secondaryEDL(bool state);
    void secondaryPMResinN(bool state);
    void eud(bool state);
    void platformID();
    void headsetDisconnect(bool state);

private:
    ReceiveInterface* _receiver;
    SendInterface*    _sender;
    bool              _ready;
};

} // namespace qtac
