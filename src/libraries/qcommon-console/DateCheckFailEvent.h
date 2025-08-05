#ifndef DATECHECKFAILEVENT_H
#define DATECHECKFAILEVENT_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "QCommonConsoleGlobal.h"

#include <QEvent>
#include <QString>

const QEvent::Type kDateCheckFailEvent(static_cast<QEvent::Type>(QEvent::User + 1062));

class QCOMMONCONSOLE_EXPORT DateCheckFailEvent :
	public QEvent
{
public:
	DateCheckFailEvent(const QString& why) :
		QEvent(kDateCheckFailEvent),
		_why(why) {}

	QString						_why;
};

#endif // DATECHECKFAILEVENT_H
