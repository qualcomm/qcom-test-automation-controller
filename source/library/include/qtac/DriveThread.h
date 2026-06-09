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
// Qt-free reimplementation of DriveThread from qcommon-console.
// Replaces QThread inheritance with an owned std::thread.

#pragma once

#include <qtac/ByteArray.h>
#include <qtac/SendInterface.h>

#include <atomic>
#include <map>
#include <mutex>
#include <thread>

namespace qtac {

class ProtocolInterface;

class DriveThread : public SendInterface
{
public:
    DriveThread();
    virtual ~DriveThread();

    // Returns true while the drive thread's run() loop is active.
    bool weAreRunning()
    {
        std::lock_guard<std::mutex> lock(_runningMutex);
        return _running;
    }

    // Spin up the std::thread that executes run().
    void start()
    {
        _thread = std::thread([this] { run(); });
    }

    // True if the underlying thread is joinable (i.e. started and not yet joined).
    bool isRunning() const { return _thread.joinable(); }

    // Signal stop and block until the thread exits.
    void shutDown();

    std::thread::id threadId() const { return _thread.get_id(); }

    // Detach rather than join — use only when the drive thread calls close() from
    // within run() itself, where a join would deadlock.
    void detachThread()
    {
        if (_thread.joinable())
            _thread.detach();
    }

    qtac::ByteArray name() const { return _driveTrainName; }
    int             id()   const { return _driveTrainID; }

    qtac::ByteArray lastErrorMessage()
    {
        std::lock_guard<std::mutex> lock(_runningMutex);
        qtac::ByteArray result = _lastErrorMessage;
        _lastErrorMessage.clear();
        return result;
    }

    void setProtocolInterface(ProtocolInterface* protocolInterface);

    virtual void run() = 0;

    // SendInterface
    virtual uint32_t send(const qtac::ByteArray& sendMe, const Arguments& arguments, bool command,
                          ReceiveInterface* receiveInterface, bool store = true) = 0;
    virtual void addDelay(uint32_t delayInMilliSeconds, ReceiveInterface* receiveInterface) override;
    virtual void addLogComment(const qtac::ByteArray& comment) override;
    virtual void addEndTransaction(ReceiveInterface* receiveInterface) override;

    // Clears the running flag; returns the previous value.
    // Public so concrete drive threads can call it from within run() during
    // self-initiated shutdown before detachThread().
    bool stopRunning()
    {
        std::lock_guard<std::mutex> lock(_runningMutex);
        bool result = _running;
        _running    = false;
        return result;
    }

protected:
    void startRunning()
    {
        std::lock_guard<std::mutex> lock(_runningMutex);
        _running = true;
    }

    static std::atomic<int> _driveTrainIDs;

    int                     _driveTrainID{0};
    ProtocolInterface*      _protocolInterface{nullptr};
    qtac::ByteArray         _driveTrainName{"<unnamed>"};
    qtac::ByteArray         _lastErrorMessage;
    std::thread             _thread;

private:
    std::mutex  _runningMutex;
    bool        _running{false};
};

using DriveTrains = std::map<int, DriveThread*>;

} // namespace qtac
