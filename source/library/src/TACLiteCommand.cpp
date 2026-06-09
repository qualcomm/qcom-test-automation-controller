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
static const std::string kVersionCommand               {"Version"};
static const std::string kGetNameCommand               {"Get Name"};
static const std::string kSetNameCommand               {"Set Name"};
static const std::string kGetUUIDCommand               {"Get UUID"};
static const std::string kGetPlatformIDCommand         {"Get Platform ID"};
static const std::string kGetResetCountCommand         {"Get Reset Count"};
static const std::string kClearResetCountCommand       {"Clear Reset Count"};
static const std::string kSetPinCommand                {"SetPin"};
static const std::string kSetBatteryCommand            {"Battery"};
static const std::string kSetUSB0Command               {"USB0"};
static const std::string kSetUSB1Command               {"USB1"};
static const std::string kSetPowerKeyCommand           {"Power Key"};
static const std::string kSetVolumeUpCommand           {"Volume Up"};
static const std::string kSetVolumeDownCommand         {"Volume Down"};
static const std::string kSetExternalPowerControlCommand {"External Power Control"};
static const std::string kSetDisconnectUIM1Command     {"Disconnect UIM1"};
static const std::string kSetDisconnectUIM2Command     {"Disconnect UIM2"};
static const std::string kSetForcePSHoldHighCommand    {"Force PS Hold High"};
static const std::string kSetDisconnectSDCardCommand   {"Disconnect SDCARD"};
static const std::string kSetPrimaryEDLCommand         {"Primary EDL"};
static const std::string kSetSecondaryEDLCommand       {"Secondary EDL"};
static const std::string kSecondaryPM_RESIN_NCommand   {"Secondary PM RESIN N"};
static const std::string kSetEUDCommand                {"EUD"};
static const std::string kSetHeadsetDisconnectCommand  {"Headset Disconnect"};
static const std::string kI2CReadRegisterCommand       {"I2C Read Register"};
static const std::string kI2CWriteRegisterCommand      {"I2C Write Register"};

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
    send(qtac::ByteArray(kVersionCommand), Arguments(), false);
}

void TACLiteCommand::name()
{
    send(qtac::ByteArray(kGetNameCommand), Arguments(), false);
}

void TACLiteCommand::uuid()
{
    send(qtac::ByteArray(kGetUUIDCommand), Arguments(), false, false);
}

void TACLiteCommand::setPinState(uint16_t pin, bool state)
{
    Arguments args;
    args.push_back(state);
    args.push_back(static_cast<uint32_t>(pin));
    send(qtac::ByteArray(kSetPinCommand), args, false, false);
}

void TACLiteCommand::battery(bool state)
{
    Arguments args;
    args.push_back(state);
    send(qtac::ByteArray(kSetBatteryCommand), args, false, false);
}

void TACLiteCommand::usb0(bool state)
{
    Arguments args;
    args.push_back(state);
    send(qtac::ByteArray(kSetUSB0Command), args, false);
}

void TACLiteCommand::usb1(bool state)
{
    Arguments args;
    args.push_back(state);
    send(qtac::ByteArray(kSetUSB1Command), args, false);
}

void TACLiteCommand::externalPowerControl(bool state)
{
    Arguments args;
    args.push_back(state);
    send(qtac::ByteArray(kSetExternalPowerControlCommand), args, false);
}

void TACLiteCommand::powerKey(bool state)
{
    Arguments args;
    args.push_back(state);
    send(qtac::ByteArray(kSetPowerKeyCommand), args, false);
}

void TACLiteCommand::volumeUp(bool state)
{
    Arguments args;
    args.push_back(state);
    send(qtac::ByteArray(kSetVolumeUpCommand), args, false);
}

void TACLiteCommand::volumeDown(bool state)
{
    Arguments args;
    args.push_back(state);
    send(qtac::ByteArray(kSetVolumeDownCommand), args, false);
}

void TACLiteCommand::setName(const qtac::ByteArray& newName)
{
    Arguments args;
    args.push_back(newName.toStdString());
    send(qtac::ByteArray(kSetNameCommand), args, false);
}

void TACLiteCommand::getResetCount()
{
    send(qtac::ByteArray(kGetResetCountCommand), Arguments(), false);
}

void TACLiteCommand::clearResetCount()
{
    send(qtac::ByteArray(kClearResetCountCommand), Arguments(), false);
}

void TACLiteCommand::i2CReadRegister(uint32_t addr, uint32_t reg)
{
    Arguments args;
    args.push_back(addr);
    args.push_back(reg);
    send(qtac::ByteArray(kI2CReadRegisterCommand), args, false);
}

void TACLiteCommand::i2CWriteRegister(uint32_t addr, uint32_t reg, uint32_t data)
{
    Arguments args;
    args.push_back(addr);
    args.push_back(reg);
    args.push_back(data);
    send(qtac::ByteArray(kI2CWriteRegisterCommand), args, false);
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
    send(qtac::ByteArray(kSetDisconnectUIM1Command), args, false);
}

void TACLiteCommand::disconnectUIM2Button(bool state)
{
    Arguments args;
    args.push_back(state);
    send(qtac::ByteArray(kSetDisconnectUIM2Command), args, false);
}

void TACLiteCommand::forcePSHoldHigh(bool state)
{
    Arguments args;
    args.push_back(state);
    send(qtac::ByteArray(kSetForcePSHoldHighCommand), args, false);
}

void TACLiteCommand::disconnectSDCard(bool state)
{
    Arguments args;
    args.push_back(state);
    send(qtac::ByteArray(kSetDisconnectSDCardCommand), args, false);
}

void TACLiteCommand::primaryEDL(bool state)
{
    Arguments args;
    args.push_back(state);
    send(qtac::ByteArray(kSetPrimaryEDLCommand), args, false);
}

void TACLiteCommand::secondaryEDL(bool state)
{
    Arguments args;
    args.push_back(state);
    send(qtac::ByteArray(kSetSecondaryEDLCommand), args, false);
}

void TACLiteCommand::secondaryPMResinN(bool state)
{
    Arguments args;
    args.push_back(state);
    send(qtac::ByteArray(kSecondaryPM_RESIN_NCommand), args, false);
}

void TACLiteCommand::eud(bool state)
{
    Arguments args;
    args.push_back(state);
    send(qtac::ByteArray(kSetEUDCommand), args, false);
}

void TACLiteCommand::platformID()
{
    send(qtac::ByteArray(kGetPlatformIDCommand), Arguments(), false);
}

void TACLiteCommand::headsetDisconnect(bool state)
{
    Arguments args;
    args.push_back(state);
    send(qtac::ByteArray(kSetHeadsetDisconnectCommand), args, false);
}

} // namespace qtac
