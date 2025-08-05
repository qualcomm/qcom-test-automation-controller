#ifndef QUITAPPEVENT_H
#define QUITAPPEVENT_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "QCommonConsoleGlobal.h"

#include <QEvent>

const QEvent::Type kQuitAppEvent(static_cast<QEvent::Type>(QEvent::User + 1));

class QCOMMONCONSOLE_EXPORT QuitAppEvent :
	public QEvent
{
public:
	QuitAppEvent();
};

#endif // QUITTACEVENT_H
