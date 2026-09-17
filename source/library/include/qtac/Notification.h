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

// Qt-free port of upstream Notification class.
// Uses std::chrono::system_clock instead of QDateTime.
// NotificationLevel is declared in TACDriveThread.h.

#pragma once

#include <qtac/String.h>
#include <qtac/TACDriveThread.h>

#include <chrono>
#include <cstdint>

// -----------------------------------------------------------------------
// Notification
//
// Immutable value type carrying a message, severity level, identifier,
// timestamp, and occurrence count.  Mirrors the upstream Notification
// class but replaces QDateTime with std::chrono::system_clock.
// -----------------------------------------------------------------------

namespace qtac {

struct Notification
{
    using Clock     = std::chrono::system_clock;
    using TimePoint = std::chrono::time_point<Clock>;

    Notification() = default;

    Notification(const qtac::String& message, NotificationLevel level, uint64_t id = 0)
        : _message(message)
        , _level(level)
        , _id(id)
        , _timestamp(Clock::now())
        , _occurrenceCount(1)
    {}

    qtac::String      getMessage()         const { return _message;         }
    NotificationLevel getLevel()           const { return _level;           }
    uint64_t          getId()              const { return _id;              }
    TimePoint         getTimestamp()       const { return _timestamp;       }
    uint32_t          getOccurrenceCount() const { return _occurrenceCount; }

    // Increment the occurrence counter and refresh the timestamp.
    void addOccurrence()
    {
        ++_occurrenceCount;
        _timestamp = Clock::now();
    }

private:
    qtac::String      _message;
    NotificationLevel _level{NotificationLevel::Info};
    uint64_t          _id{0};
    TimePoint         _timestamp;
    uint32_t          _occurrenceCount{0};
};

} // namespace qtac
