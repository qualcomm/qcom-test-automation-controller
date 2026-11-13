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

// Author: Biswajit Roy

#include <qtac/TACPIC32CXCommand.h>
#include <qtac/ReceiveInterface.h>
#include <qtac/SendInterface.h>

#include <cassert>

// PIC32CX command string constants
static const qtac::ByteArray kVersionCommand              {"Version"};
static const qtac::ByteArray kGetNameCommand              {"Get Name"};
static const qtac::ByteArray kGetUUIDCommand              {"Get UUID"};
static const qtac::ByteArray kSetNameCommand              {"Set Name"};
static const qtac::ByteArray kPIC32CXPlatformIDCommand    {"*IDN?"};
static const qtac::ByteArray kPIC32CXSetPinCommand        {"CONF:DIG:ON"};
static const qtac::ByteArray kPIC32CXClearBufferCommand   {"echo 1"};

namespace qtac {

TACPIC32CXCommand::TACPIC32CXCommand(SendInterface* sender, ReceiveInterface* receiver)
    : _receiver(receiver)
    , _sender(sender)
    , _ready(false)
{
    assert(_receiver != nullptr);
    assert(_sender   != nullptr);
    _ready = sender->ready();
}

TACPIC32CXCommand::~TACPIC32CXCommand()
{
    addEndTransaction();
}

void TACPIC32CXCommand::version()
{
    send(kVersionCommand, Arguments(), false, false);
}

void TACPIC32CXCommand::name()
{
    send(kGetNameCommand, Arguments(), false, false);
}

void TACPIC32CXCommand::uuid()
{
    send(kGetUUIDCommand, Arguments(), false, false);
}

void TACPIC32CXCommand::setPinState(uint16_t pin, bool state)
{
    Arguments args;
    args.push_back(state);
    args.push_back(static_cast<uint32_t>(pin));
    send(kPIC32CXSetPinCommand, args, false, false);
}

void TACPIC32CXCommand::setName(const qtac::ByteArray& newName)
{
    Arguments args;
    args.push_back(newName.toStdString());
    send(kSetNameCommand, args, false, false);
}

void TACPIC32CXCommand::platformID()
{
    send(kPIC32CXPlatformIDCommand, Arguments(), false, false);
}

void TACPIC32CXCommand::clearBuffer()
{
    send(kPIC32CXClearBufferCommand, Arguments(), false, false);
}

void TACPIC32CXCommand::send(const qtac::ByteArray& command, const Arguments& arguments,
                              bool console, bool store)
{
    _sender->send(command, arguments, console, _receiver, store);
}

void TACPIC32CXCommand::addDelay(uint32_t delayInMilliSeconds)
{
    _sender->addDelay(delayInMilliSeconds, _receiver);
}

void TACPIC32CXCommand::addLogComment(const qtac::ByteArray& comment)
{
    _sender->addLogComment(comment);
}

void TACPIC32CXCommand::addEndTransaction()
{
    _sender->addEndTransaction(_receiver);
}

} // namespace qtac
