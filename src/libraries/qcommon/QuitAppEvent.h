#ifndef QUITAPPEVENT_H
#define QUITAPPEVENT_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonGlobal.h"

#include <QEvent>

const QEvent::Type kQuitAppEvent(static_cast<QEvent::Type>(QEvent::User + 1));

class QCOMMON_EXPORT QuitAppEvent :
	public QEvent
{
public:
	QuitAppEvent();
};

#endif // QUITAPPEVENT_H
