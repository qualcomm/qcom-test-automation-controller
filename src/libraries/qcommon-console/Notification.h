#ifndef NOTIFICATION_H
#define NOTIFICATION_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"

#include <chrono>
#include <cstdint>
#include <string>

enum QCOMMONCONSOLE_EXPORT NotificationLevel
{
    eDebugNotification = 0,
    eInfoNotification,
	eWarnNotification,
	eErrorNotification
};

// Could not find a sane way to manage these variables
const uint8_t kProgressActive(0);
const uint8_t kProgressMax(100);
const uint32_t kNotificationLabelWidth(320);
const uint32_t kNotificationLabelHeight(80);

class QCOMMONCONSOLE_EXPORT Notification
{
public:
    Notification(const std::string& message, const NotificationLevel level, const uint64_t id = 0)
    {
        _message = message;
        _level = level;
        _id = id;
        _timestamp = std::chrono::system_clock::now();
        _occurrenceCount = 1;
    }

    std::string getMessage() const
    {
        return _message;
    }

    NotificationLevel getLevel() const
    {
        return _level;
    }

    uint64_t getId() const
    {
        return _id;
    }

    std::chrono::system_clock::time_point getTimestamp() const
    {
        return _timestamp;
    }

    uint32_t getOccurrenceCount() const
    {
        return _occurrenceCount;
    }

    void addOccurrence()
    {
        _occurrenceCount++;
        _timestamp = std::chrono::system_clock::now();
    }

private:
    std::string                                 _message;
    NotificationLevel                           _level;
    uint64_t                                    _id;
    std::chrono::system_clock::time_point       _timestamp;
    uint32_t                                    _occurrenceCount;
};

#endif // NOTIFICATION_H
