#ifndef NOTIFICATION_H
#define NOTIFICATION_H

// Confidential and Proprietary Qualcomm Technologies, Inc.

// NO PUBLIC DISCLOSURE PERMITTED:  Please report postings of this software on public servers or websites
// to: DocCtrlAgent@qualcomm.com.

// RESTRICTED USE AND DISCLOSURE:
// This software contains confidential and proprietary information and is not to be used, copied, reproduced, modified
// or distributed, in whole or in part, nor its contents revealed in any manner, without the express written permission
// of Qualcomm Technologies, Inc.

// Qualcomm is a trademark of QUALCOMM Incorporated, registered in the United States and other countries. All
// QUALCOMM Incorporated trademarks are used with permission.

// This software may be subject to U.S. and international export, re-export, or transfer laws.  Diversion contrary to U.S.
// and international law is strictly prohibited.

// Qualcomm Technologies, Inc.
// 5775 Morehouse Drive
// San Diego, CA 92121 U.S.A.
// Copyright 2024-2025 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
