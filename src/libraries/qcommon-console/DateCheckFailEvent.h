#ifndef DATECHECKFAILEVENT_H
#define DATECHECKFAILEVENT_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"

#include <QEvent>

#include <string>

const QEvent::Type kDateCheckFailEvent(static_cast<QEvent::Type>(QEvent::User + 1062));

class QCOMMONCONSOLE_EXPORT DateCheckFailEvent :
	public QEvent
{
public:
	DateCheckFailEvent(const std::string& why) :
		QEvent(kDateCheckFailEvent),
		_why(why) {}

	std::string					_why;
};

#endif // DATECHECKFAILEVENT_H
