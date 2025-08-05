#ifndef NOTIFICATION_H
#define NOTIFICATION_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
    Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "QCommonConsoleGlobal.h"

// Qt
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
    Notification(const QString& message, const NotificationLevel level)
    {
        _message = message;
        _level = level;
    }

    QString getMessage() const
    {
        return _message;
    }

    NotificationLevel getLevel() const
    {
        return _level;
    }

private:
    QString                     _message;
    NotificationLevel           _level;
};

#endif // NOTIFICATION_H
