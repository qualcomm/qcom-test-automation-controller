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

#include <qtac/TACPSOCCommand.h>
#include <qtac/ReceiveInterface.h>
#include <qtac/SendInterface.h>

#include <cassert>
#include <iomanip>
#include <sstream>
#include <stdexcept>

// Command string constants
static const std::string kVersionCommand          {"Version"};
static const std::string kGetNameCommand          {"Get Name"};
static const std::string kSetNameCommand          {"Set Name"};
static const std::string kGetUUIDCommand          {"Get UUID"};
static const std::string kGetPlatformIDCommand    {"Get Platform ID"};
static const std::string kGetResetCountCommand    {"Get Reset Count"};
static const std::string kClearResetCountCommand  {"Clear Reset Count"};
static const std::string kSetPinCommand           {"SetPin"};
static const std::string kI2CReadRegisterCommand  {"I2C Read Register"};
static const std::string kI2CReadRegisterValueCommand {"I2C Read Register Value"};
static const std::string kI2CWriteRegisterCommand {"I2C Write Register"};

namespace qtac {

TACPSOCCommand::TACPSOCCommand(SendInterface* sender, ReceiveInterface* receiver)
    : _receiver(receiver)
    , _sender(sender)
    , _ready(false)
{
    assert(_receiver != nullptr);
    assert(_sender   != nullptr);
    _ready = sender->ready();
}

TACPSOCCommand::~TACPSOCCommand()
{
    addEndTransaction();
}

void TACPSOCCommand::version()
{
    send(qtac::ByteArray(kVersionCommand), Arguments(), false, false);
}

void TACPSOCCommand::name()
{
    send(qtac::ByteArray(kGetNameCommand), Arguments(), false, false);
}

void TACPSOCCommand::uuid()
{
    send(qtac::ByteArray(kGetUUIDCommand), Arguments(), false, false);
}

void TACPSOCCommand::setPinState(uint16_t pin, bool state)
{
    Arguments args;
    args.push_back(state);
    args.push_back(static_cast<uint32_t>(pin));
    send(qtac::ByteArray(kSetPinCommand), args, false, false);
}

void TACPSOCCommand::setName(const qtac::ByteArray& newName)
{
    Arguments args;
    args.push_back(newName.toStdString());
    send(qtac::ByteArray(kSetNameCommand), args, false, false);
}

void TACPSOCCommand::getResetCount()
{
    send(qtac::ByteArray(kGetResetCountCommand), Arguments(), false, false);
}

void TACPSOCCommand::clearResetCount()
{
    send(qtac::ByteArray(kClearResetCountCommand), Arguments(), false, false);
}

// Helper: format a byte value as "0xNN" hex string.
static std::string toHexByte(uint32_t value)
{
    std::ostringstream oss;
    oss << "0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << value;
    return oss.str();
}

void TACPSOCCommand::i2CReadRegister(uint32_t addr, uint32_t reg)
{
    if (addr > 0xff)
        throw std::out_of_range("Error: invalid address");
    if (reg > 0xff)
        throw std::out_of_range("Error: invalid register");

    const std::string addrStr = toHexByte(addr);
    const std::string regStr  = toHexByte(reg);

    Arguments args;
    args.push_back(addrStr + " " + regStr);
    send(qtac::ByteArray(kI2CReadRegisterCommand), args, false, false);

    addDelay(500);

    args.clear();
    send(qtac::ByteArray(kI2CReadRegisterValueCommand), args, false, false);
}

void TACPSOCCommand::i2CWriteRegister(uint32_t addr, uint32_t reg, uint32_t data)
{
    if (addr > 0xff)
        throw std::out_of_range("Error: invalid address");
    if (reg > 0xff)
        throw std::out_of_range("Error: invalid register");
    if (data > 0xff)
        throw std::out_of_range("Error: invalid data");

    const std::string addrStr = toHexByte(addr);
    const std::string regStr  = toHexByte(reg);
    const std::string dataStr = toHexByte(data);

    Arguments args;
    args.push_back(addrStr + " " + regStr + " " + dataStr);
    send(qtac::ByteArray(kI2CWriteRegisterCommand), args, false, false);
}

void TACPSOCCommand::platformID()
{
    send(qtac::ByteArray(kGetPlatformIDCommand), Arguments(), false, false);
}

void TACPSOCCommand::send(const qtac::ByteArray& command, const Arguments& arguments,
                           bool console, bool store)
{
    _sender->send(command, arguments, console, _receiver, store);
}

void TACPSOCCommand::addDelay(uint32_t delayInMilliSeconds)
{
    _sender->addDelay(delayInMilliSeconds, _receiver);
}

void TACPSOCCommand::addLogComment(const qtac::ByteArray& comment)
{
    _sender->addLogComment(comment);
}

void TACPSOCCommand::addEndTransaction()
{
    _sender->addEndTransaction(_receiver);
}

} // namespace qtac
