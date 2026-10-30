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
// Qt-free reimplementation of ThreadedLog from qcommon-console.
// QThread          -> std::thread (owned)
// QSharedPointer   -> std::shared_ptr
// QRecursiveMutex  -> std::recursive_mutex
// QString          -> qtac::ByteArray
// QStringList      -> std::vector<qtac::ByteArray>
// QFile            -> std::ofstream
// QDateTime        -> std::chrono + strftime

#pragma once

#include <qtac/ByteArray.h>

#include <atomic>
#include <fstream>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace qtac {

class _ThreadedLog;
using ThreadedLog = std::shared_ptr<_ThreadedLog>;

class _ThreadedLog
{
public:
    _ThreadedLog() = default;
    ~_ThreadedLog();

    static ThreadedLog      createThreadedLog();
    static qtac::ByteArray  createLogName(const qtac::ByteArray& prefix);

    void open(const qtac::ByteArray& filePath);
    bool isOpen();
    void close();

    qtac::ByteArray currentLogPath() const { return _currentLogPath; }

    void addLogEntry(const qtac::ByteArray& logEntry);

    void run();

private:
    std::atomic<bool>               _running{false};
    std::atomic<bool>               _starting{false};
    std::recursive_mutex            _mutex;
    std::vector<qtac::ByteArray>    _logEntries;
    qtac::ByteArray                 _currentLogPath;
    std::ofstream                   _logFile;
    std::thread                     _thread;
};

} // namespace qtac
