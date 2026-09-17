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

#include <qtac/ThreadedLog.h>

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace qtac {

_ThreadedLog::~_ThreadedLog()
{
    close();
}

void _ThreadedLog::open(const qtac::ByteArray& filePath)
{
    _currentLogPath = filePath;

    _starting = true;
    _thread = std::thread([this] { run(); });

    // Wait for run() to signal that the file is open (or failed).
    while (_starting)
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

bool _ThreadedLog::isOpen()
{
    return _running;
}

void _ThreadedLog::close()
{
    if (_running)
    {
        _running = false;
        _currentLogPath = qtac::ByteArray();
    }

    if (_thread.joinable())
        _thread.join();
}

void _ThreadedLog::addLogEntry(const qtac::ByteArray& logEntry)
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);
    _logEntries.push_back(logEntry);
}

void _ThreadedLog::run()
{
    _logFile.open(_currentLogPath.toStdString(), std::ios::out | std::ios::binary);

    _starting = false;
    _running  = _logFile.is_open();

    while (_running)
    {
        qtac::ByteArray entry;

        {
            std::lock_guard<std::recursive_mutex> lock(_mutex);
            if (!_logEntries.empty())
            {
                entry = std::move(_logEntries.front());
                _logEntries.erase(_logEntries.begin());
            }
        }

        if (!entry.isEmpty())
        {
            _logFile << entry.toStdString();
            _logFile.flush();
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    // Drain any remaining entries after _running is cleared.
    std::lock_guard<std::recursive_mutex> lock(_mutex);
    for (const auto& entry : _logEntries)
        _logFile << entry.toStdString();
    _logEntries.clear();

    _logFile.close();
}

ThreadedLog _ThreadedLog::createThreadedLog()
{
    return std::make_shared<_ThreadedLog>();
}

qtac::ByteArray _ThreadedLog::createLogName(const qtac::ByteArray& prefix)
{
    using namespace std::chrono;
    auto now   = system_clock::to_time_t(system_clock::now());
    std::tm tm{};
#if defined(_WIN32)
    localtime_s(&tm, &now);
#else
    localtime_r(&now, &tm);
#endif
    std::ostringstream ss;
    ss << prefix.toStdString() << std::put_time(&tm, "%m%d_%H%M%S") << ".log";
    return qtac::ByteArray(ss.str().c_str());
}

} // namespace qtac
