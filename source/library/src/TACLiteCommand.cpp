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

#include <qtac/TACLiteCommand.h>
#include <qtac/ReceiveInterface.h>
#include <qtac/SendInterface.h>

// Command string constants (mirrors TACCommands.h)
static const qtac::ByteArray kVersionCommand               {"Version"};
static const qtac::ByteArray kGetNameCommand               {"Get Name"};
static const qtac::ByteArray kSetNameCommand               {"Set Name"};
static const qtac::ByteArray kGetUUIDCommand               {"Get UUID"};
static const qtac::ByteArray kGetPlatformIDCommand         {"Get Platform ID"};
static const qtac::ByteArray kGetResetCountCommand         {"Get Reset Count"};
static const qtac::ByteArray kClearResetCountCommand       {"Clear Reset Count"};
static const qtac::ByteArray kSetPinCommand                {"SetPin"};
static const qtac::ByteArray kSetBatteryCommand            {"Battery"};
static const qtac::ByteArray kSetUSB0Command               {"USB0"};
static const qtac::ByteArray kSetUSB1Command               {"USB1"};
static const qtac::ByteArray kSetPowerKeyCommand           {"Power Key"};
static const qtac::ByteArray kSetVolumeUpCommand           {"Volume Up"};
static const qtac::ByteArray kSetVolumeDownCommand         {"Volume Down"};
static const qtac::ByteArray kSetExternalPowerControlCommand {"External Power Control"};
static const qtac::ByteArray kSetDisconnectUIM1Command     {"Disconnect UIM1"};
static const qtac::ByteArray kSetDisconnectUIM2Command     {"Disconnect UIM2"};
static const qtac::ByteArray kSetForcePSHoldHighCommand    {"Force PS Hold High"};
static const qtac::ByteArray kSetDisconnectSDCardCommand   {"Disconnect SDCARD"};
static const qtac::ByteArray kSetPrimaryEDLCommand         {"Primary EDL"};
static const qtac::ByteArray kSetSecondaryEDLCommand       {"Secondary EDL"};
static const qtac::ByteArray kSecondaryPM_RESIN_NCommand   {"Secondary PM RESIN N"};
static const qtac::ByteArray kSetEUDCommand                {"EUD"};
static const qtac::ByteArray kSetHeadsetDisconnectCommand  {"Headset Disconnect"};
static const qtac::ByteArray kI2CReadRegisterCommand       {"I2C Read Register"};
static const qtac::ByteArray kI2CWriteRegisterCommand      {"I2C Write Register"};

namespace qtac {

TACLiteCommand::TACLiteCommand(SendInterface* sender, ReceiveInterface* receiver)
    : _receiver(receiver)
    , _sender(sender)
    , _ready(sender->ready())
{
}

TACLiteCommand::~TACLiteCommand()
{
    addEndTransaction();
}

void TACLiteCommand::version()
{
    send(kVersionCommand, Arguments(), false);
}

void TACLiteCommand::name()
{
    send(kGetNameCommand, Arguments(), false);
}

void TACLiteCommand::uuid()
{
    send(kGetUUIDCommand, Arguments(), false, false);
}

void TACLiteCommand::setPinState(uint16_t pin, bool state)
{
    Arguments args;
    args.push_back(state);
    args.push_back(static_cast<uint32_t>(pin));
    send(kSetPinCommand, args, false, false);
}

void TACLiteCommand::battery(bool state)
{
    Arguments args;
    args.push_back(state);
    send(kSetBatteryCommand, args, false, false);
}

void TACLiteCommand::usb0(bool state)
{
    Arguments args;
    args.push_back(state);
    send(kSetUSB0Command, args, false);
}

void TACLiteCommand::usb1(bool state)
{
    Arguments args;
    args.push_back(state);
    send(kSetUSB1Command, args, false);
}

void TACLiteCommand::externalPowerControl(bool state)
{
    Arguments args;
    args.push_back(state);
    send(kSetExternalPowerControlCommand, args, false);
}

void TACLiteCommand::powerKey(bool state)
{
    Arguments args;
    args.push_back(state);
    send(kSetPowerKeyCommand, args, false);
}

void TACLiteCommand::volumeUp(bool state)
{
    Arguments args;
    args.push_back(state);
    send(kSetVolumeUpCommand, args, false);
}

void TACLiteCommand::volumeDown(bool state)
{
    Arguments args;
    args.push_back(state);
    send(kSetVolumeDownCommand, args, false);
}

void TACLiteCommand::setName(const qtac::ByteArray& newName)
{
    Arguments args;
    args.push_back(newName.toStdString());
    send(kSetNameCommand, args, false);
}

void TACLiteCommand::getResetCount()
{
    send(kGetResetCountCommand, Arguments(), false);
}

void TACLiteCommand::clearResetCount()
{
    send(kClearResetCountCommand, Arguments(), false);
}

void TACLiteCommand::i2CReadRegister(uint32_t addr, uint32_t reg)
{
    Arguments args;
    args.push_back(addr);
    args.push_back(reg);
    send(kI2CReadRegisterCommand, args, false);
}

void TACLiteCommand::i2CWriteRegister(uint32_t addr, uint32_t reg, uint32_t data)
{
    Arguments args;
    args.push_back(addr);
    args.push_back(reg);
    args.push_back(data);
    send(kI2CWriteRegisterCommand, args, false);
}

void TACLiteCommand::send(const qtac::ByteArray& command, const Arguments& arguments,
                           bool console, bool store)
{
    _sender->send(command, arguments, console, _receiver, store);
}

void TACLiteCommand::addDelay(uint32_t delayInMilliSeconds)
{
    _sender->addDelay(delayInMilliSeconds, _receiver);
}

void TACLiteCommand::addLogComment(const qtac::ByteArray& comment)
{
    _sender->addLogComment(comment);
}

void TACLiteCommand::addEndTransaction()
{
    _sender->addEndTransaction(_receiver);
}

void TACLiteCommand::disconnectUIM1Button(bool state)
{
    Arguments args;
    args.push_back(state);
    send(kSetDisconnectUIM1Command, args, false);
}

void TACLiteCommand::disconnectUIM2Button(bool state)
{
    Arguments args;
    args.push_back(state);
    send(kSetDisconnectUIM2Command, args, false);
}

void TACLiteCommand::forcePSHoldHigh(bool state)
{
    Arguments args;
    args.push_back(state);
    send(kSetForcePSHoldHighCommand, args, false);
}

void TACLiteCommand::disconnectSDCard(bool state)
{
    Arguments args;
    args.push_back(state);
    send(kSetDisconnectSDCardCommand, args, false);
}

void TACLiteCommand::primaryEDL(bool state)
{
    Arguments args;
    args.push_back(state);
    send(kSetPrimaryEDLCommand, args, false);
}

void TACLiteCommand::secondaryEDL(bool state)
{
    Arguments args;
    args.push_back(state);
    send(kSetSecondaryEDLCommand, args, false);
}

void TACLiteCommand::secondaryPMResinN(bool state)
{
    Arguments args;
    args.push_back(state);
    send(kSecondaryPM_RESIN_NCommand, args, false);
}

void TACLiteCommand::eud(bool state)
{
    Arguments args;
    args.push_back(state);
    send(kSetEUDCommand, args, false);
}

void TACLiteCommand::platformID()
{
    send(kGetPlatformIDCommand, Arguments(), false);
}

void TACLiteCommand::headsetDisconnect(bool state)
{
    Arguments args;
    args.push_back(state);
    send(kSetHeadsetDisconnectCommand, args, false);
}

} // namespace qtac
