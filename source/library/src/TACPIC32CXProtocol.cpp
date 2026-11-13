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

#include <qtac/TACPIC32CXProtocol.h>
#include <qtac/TACPIC32CXCoder.h>
#include <qtac/TACDriveThread.h>
#include <qtac/StringUtilities.h>

#include <cassert>
#include <chrono>

namespace qtac {

static uint64_t tickCount()
{
    using namespace std::chrono;
    return static_cast<uint64_t>(
        duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count());
}

TACPIC32CXProtocol::TACPIC32CXProtocol()
{
    setFrameCoder(new TACPIC32CXCoder);
}

TACPIC32CXProtocol::~TACPIC32CXProtocol()
{
    delete _frameCoder;
    _frameCoder = nullptr;
}

void TACPIC32CXProtocol::setTACDriveTrain(TACDriveThread* tacDriveTrain)
{
    assert(tacDriveTrain != nullptr);
    _tacDriveTrain = tacDriveTrain;
}

uint32_t TACPIC32CXProtocol::sendCommand(const qtac::ByteArray& command,
                                          const Arguments&   arguments,
                                          bool               console,
                                          ReceiveInterface*  receiveInterface,
                                          bool               shouldStore)
{
    if (command.isEmpty())
        return kBadQueueValue;

    const uint32_t result = getNextSendID();

    FramePackage framePackage = makeFramePackage();
    framePackage->packetID         = result;
    framePackage->request          = command;
    framePackage->arguments        = arguments;
    framePackage->requestHash      = arrayHash(command);
    framePackage->console          = console;
    framePackage->shouldStore      = shouldStore;
    framePackage->tickcount        = tickCount();
    framePackage->receiveInterface = receiveInterface;

    if (_frameCoder)
        framePackage->codedRequest = _frameCoder->encode(command, arguments);
    else
        framePackage->codedRequest = command;

    ProtocolInterface::queueCommand(framePackage);
    return result;
}

void TACPIC32CXProtocol::endTransaction(ReceiveInterface* receiveInterface)
{
    ProtocolInterface::queueEndTransaction(receiveInterface);
}

void TACPIC32CXProtocol::sendHelpCommand()
{
}

void TACPIC32CXProtocol::receive(FramePackage& framePackage)
{
    _tacDriveTrain->receive(framePackage);
}

void TACPIC32CXProtocol::idle()
{
}

// -----------------------------------------------------------------------
// frameComplete
//
// Non-empty tokens are accumulated; the empty-string sentinel triggers
// the commit.  All PIC32CX frames are considered valid (original comment:
// "Consider all pic32cx frames to be valid").
// -----------------------------------------------------------------------
void TACPIC32CXProtocol::frameComplete(const qtac::ByteArray& completedFrame)
{
    if (!completedFrame.isEmpty())
    {
        _responseLines.push_back(completedFrame);
    }
    else
    {
        FramePackage& fp = pendingFramePackage();
        if (fp)
        {
            fp->responses = _responseLines;
            fp->valid     = true;   // all PIC32CX frames considered valid

            if (fp->receiveInterface)
                fp->receiveInterface->receive(fp);
            else
                _tacDriveTrain->receive(fp);
        }

        clearPendingFrame();
        _responseLines.clear();
    }
}

void TACPIC32CXProtocol::badFrame(const qtac::ByteArray& /*completedFrame*/)
{
}

} // namespace qtac
