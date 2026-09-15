#ifndef NOTIFICATION_H
#define NOTIFICATION_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"

// Qt
#include <QDateTime>
#include <QString>

enum QCOMMONCONSOLE_EXPORT NotificationLevel
{
    eDebugNotification = 0,
    eInfoNotification,
	eWarnNotification,
	eErrorNotification
};

// Could not find a sane way to manage these variables
const quint8 kProgressActive(0);
const quint8 kProgressMax(100);
const quint32 kNotificationLabelWidth(320);
const quint32 kNotificationLabelHeight(80);

class QCOMMONCONSOLE_EXPORT Notification
{
public:
    Notification(const QString& message, const NotificationLevel level, const quint64 id = 0)
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

    quint64 getId() const
    {
        return _id;
    }

    QDateTime getTimestamp() const
    {
        return _timestamp;
    }

    quint32 getOccurrenceCount() const
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
    quint64                      _id;
    QDateTime                   _timestamp;
    quint32                      _occurrenceCount;
};

#endif // NOTIFICATION_H
