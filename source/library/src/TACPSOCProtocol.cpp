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

#include <qtac/TACPSOCProtocol.h>
#include <qtac/TACPSOCCoder.h>
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

// Prompt string recognised in frameComplete().
static const qtac::ByteArray kCommand              {"CMD >> "};
static const qtac::ByteArray kCommandNotRecognized {"CMD: Command not recognized."};

TACPSOCProtocol::TACPSOCProtocol()
{
    setFrameCoder(new TACPSOCCoder);
}

TACPSOCProtocol::~TACPSOCProtocol()
{
    delete _frameCoder;
    _frameCoder = nullptr;
}

void TACPSOCProtocol::setTACDriveTrain(TACDriveThread* tacDriveTrain)
{
    assert(tacDriveTrain != nullptr);
    _tacDriveTrain = tacDriveTrain;
}

uint32_t TACPSOCProtocol::sendCommand(const qtac::ByteArray& command,
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

void TACPSOCProtocol::endTransaction(ReceiveInterface* receiveInterface)
{
    ProtocolInterface::queueEndTransaction(receiveInterface);
}

void TACPSOCProtocol::sendHelpCommand()
{
}

void TACPSOCProtocol::receive(FramePackage& framePackage)
{
    _tacDriveTrain->receive(framePackage);
}

void TACPSOCProtocol::idle()
{
}

// -----------------------------------------------------------------------
// frameComplete
//
// Called by the coder for each decoded token.  Non-prompt, non-empty lines
// are accumulated in _responseLines.  The empty-string sentinel triggers
// the commit: populate the pending frame package, validate it ("ok" last
// line, case-insensitive), and route it to the drive thread.
// -----------------------------------------------------------------------
void TACPSOCProtocol::frameComplete(const qtac::ByteArray& completedFrame)
{
    if (!completedFrame.isEmpty())
    {
        // Skip the prompt line itself; keep all other lines.
        if (!completedFrame.contains(kCommand))
            _responseLines.push_back(completedFrame);
    }
    else
    {
        // Sentinel — commit the packet.
        FramePackage& fp = pendingFramePackage();
        if (fp)
        {
            fp->responses = _responseLines;

            // Valid if the last non-empty line is "ok" (case-insensitive).
            fp->valid = false;
            if (!_responseLines.empty())
                fp->valid = (_responseLines.back().trimmed().toLower() == "ok");

            if (fp->receiveInterface)
                fp->receiveInterface->receive(fp);
            else
                _tacDriveTrain->receive(fp);
        }

        clearPendingFrame();
        _responseLines.clear();
    }
}

void TACPSOCProtocol::badFrame(const qtac::ByteArray& /*completedFrame*/)
{
}

} // namespace qtac
