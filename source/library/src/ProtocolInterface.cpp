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

// Authors: Michael Simpson, Biswajit Roy

#include <qtac/ProtocolInterface.h>
#include <qtac/FrameCoder.h>

#include <chrono>
#include <thread>

namespace qtac {

uint32_t ProtocolInterface::_sendID{0};

// Returns milliseconds since an arbitrary epoch using the monotonic clock —
// replaces the platform-specific tickCount() / GetTickCount() pattern.
static uint64_t tickCount()
{
    using namespace std::chrono;
    return static_cast<uint64_t>(
        duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count());
}

ProtocolInterface::~ProtocolInterface()
{
    std::lock_guard<std::recursive_mutex> lock(_outQueueMutex);
    _outboundData.clear();

    delete _frameCoder;
    _frameCoder = nullptr;
}

uint32_t ProtocolInterface::getNextSendID()
{
    std::lock_guard<std::recursive_mutex> lock(_outQueueMutex);

    if (_sendID >= kBadQueueValue - 1)
        _sendID = 1;
    else
        ++_sendID;

    return _sendID;
}

void ProtocolInterface::setFrameCoder(FrameCoder* frameCoder)
{
    _frameCoder = frameCoder;
    _frameCoder->setupCallbackFunctions(
        this,
        ProtocolInterface::frameCompleteFunc,
        ProtocolInterface::badFrameFunc);
}

void ProtocolInterface::clearPendingFrame()
{
    std::lock_guard<std::recursive_mutex> lock(_pendingFramePackageMutex);
    _pendingFramePackage.reset();
}

FramePackage& ProtocolInterface::pendingFramePackage()
{
    std::lock_guard<std::recursive_mutex> lock(_pendingFramePackageMutex);
    return _pendingFramePackage;
}

void ProtocolInterface::queueCommand(const FramePackage& framePackage)
{
    std::lock_guard<std::recursive_mutex> lock(_outQueueMutex);
    _outboundData.push_back(framePackage);
}

void ProtocolInterface::queueDelay(uint32_t delayInMilliSeconds, ReceiveInterface* receiveInterface)
{
    if (delayInMilliSeconds == 0)
        return;

    FramePackage fp = makeFramePackage();
    fp->receiveInterface       = receiveInterface;
    fp->delayInMilliSeconds    = delayInMilliSeconds;
    fp->request                = qtac::ByteArray("delay: " + std::to_string(delayInMilliSeconds));

    std::lock_guard<std::recursive_mutex> lock(_outQueueMutex);
    _outboundData.push_back(fp);
}

void ProtocolInterface::queueLogComment(const qtac::ByteArray& comment)
{
    FramePackage fp = makeFramePackage();
    fp->comment = comment;

    std::lock_guard<std::recursive_mutex> lock(_outQueueMutex);
    _outboundData.push_back(fp);
}

void ProtocolInterface::queueEndTransaction(ReceiveInterface* receiveInterface)
{
    FramePackage fp = makeFramePackage();
    fp->receiveInterface = receiveInterface;
    fp->endTransaction   = true;
    fp->request          = "End Transaction";

    std::lock_guard<std::recursive_mutex> lock(_outQueueMutex);
    _outboundData.push_back(fp);
}

FramePackage ProtocolInterface::getNextFramePackage()
{
    FramePackage result;

    bool noPendingFrame;
    {
        std::lock_guard<std::recursive_mutex> lock(_pendingFramePackageMutex);
        noPendingFrame = (_pendingFramePackage == nullptr);
    }

    if (noPendingFrame)
    {
        std::lock_guard<std::recursive_mutex> lock(_outQueueMutex);

        if (!_outboundData.empty())
        {
            FramePackage fp = _outboundData.front();
            _outboundData.erase(_outboundData.begin());

            if (fp->delayInMilliSeconds != 0)
            {
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(fp->delayInMilliSeconds));
                result = fp;
            }
            else if (fp->endTransaction || !fp->comment.empty())
            {
                result = fp;
            }
            else
            {
                fp->tickcount = tickCount();
                result = fp;

                std::lock_guard<std::recursive_mutex> plock(_pendingFramePackageMutex);
                _pendingFramePackage = fp;
            }
        }
    }

    return result;
}

uint32_t ProtocolInterface::queueSize()
{
    std::lock_guard<std::recursive_mutex> lock(_outQueueMutex);
    return static_cast<uint32_t>(_outboundData.size());
}

bool ProtocolInterface::handleReceivedData(const qtac::ByteArray& receivedData)
{
    _frameCoder->decode(receivedData);
    return true;
}

void ProtocolInterface::frameCompleteFunc(const qtac::ByteArray& completedFrame,
                                          ProtocolInterface* protocolInterface)
{
    protocolInterface->frameComplete(completedFrame);
}

void ProtocolInterface::badFrameFunc(const qtac::ByteArray& completedFrame,
                                     ProtocolInterface* protocolInterface)
{
    protocolInterface->clearPendingFrame();
    protocolInterface->badFrame(completedFrame);
}

} // namespace qtac
