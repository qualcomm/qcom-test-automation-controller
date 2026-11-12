#ifndef NOTIFICATION_H
#define NOTIFICATION_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"

// Qt
#include <QDateTime>
#include <QString>

#include <cstdint>

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
    Notification(const QString& message, const NotificationLevel level, const uint64_t id = 0)
    {
        _message = message;
        _level = level;
        _id = id;
        _timestamp = QDateTime::currentDateTime();
        _occurrenceCount = 1;
    }

    QString getMessage() const
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

    QDateTime getTimestamp() const
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
        _timestamp = QDateTime::currentDateTime();
    }

private:
    QString                     _message;
    NotificationLevel           _level;
    uint64_t                     _id;
    QDateTime                   _timestamp;
    uint32_t                     _occurrenceCount;
};

#endif // NOTIFICATION_H
