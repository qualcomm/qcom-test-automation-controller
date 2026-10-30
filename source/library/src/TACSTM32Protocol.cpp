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

// Qt-free port of TACSTM32Protocol from qcommon-console.

#include <qtac/TACSTM32Protocol.h>
#include <qtac/TACDriveThread.h>
#include <qtac/StringUtilities.h>

#include <chrono>
#include <cassert>

namespace qtac {

static uint64_t tickCount()
{
    using namespace std::chrono;
    return static_cast<uint64_t>(
        duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count());
}

TACSTM32Protocol::TACSTM32Protocol()
{
    setFrameCoder(new TACSTM32Coder);
}

TACSTM32Protocol::~TACSTM32Protocol()
{
    delete _frameCoder;
    _frameCoder = nullptr;
}

void TACSTM32Protocol::setTACDriveTrain(TACDriveThread* tacDriveTrain)
{
    assert(tacDriveTrain != nullptr);
    _tacDriveTrain = tacDriveTrain;
}

uint32_t TACSTM32Protocol::sendCommand(const qtac::ByteArray& command,
                                        const Arguments& arguments,
                                        bool console,
                                        ReceiveInterface* receiveInterface,
                                        bool shouldStore)
{
    if (command.isEmpty())
        return kBadQueueValue;

    const uint32_t result = getNextSendID();

    FramePackage fp = makeFramePackage();
    fp->packetID         = result;
    fp->request          = command;
    fp->arguments        = arguments;
    fp->requestHash      = arrayHash(command);
    fp->console          = console;
    fp->shouldStore      = shouldStore;
    fp->tickcount        = tickCount();
    fp->receiveInterface = receiveInterface;

    if (_frameCoder)
        fp->codedRequest = _frameCoder->encode(command, arguments);
    else
        fp->codedRequest = command;

    ProtocolInterface::queueCommand(fp);
    return result;
}

void TACSTM32Protocol::endTransaction(ReceiveInterface* receiveInterface)
{
    ProtocolInterface::queueEndTransaction(receiveInterface);
}

void TACSTM32Protocol::receive(FramePackage& framePackage)
{
    _tacDriveTrain->receive(framePackage);
}

void TACSTM32Protocol::idle()
{
}

void TACSTM32Protocol::frameComplete(const qtac::ByteArray& /*completedFrame*/)
{
    clearPendingFrame();
}

void TACSTM32Protocol::badFrame(const qtac::ByteArray& /*completedFrame*/)
{
}

} // namespace qtac
