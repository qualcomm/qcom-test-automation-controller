#ifndef LICENSEFAILEVENT_H
#define LICENSEFAILEVENT_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "QCommonConsoleGlobal.h"

#include <QEvent>
#include <QString>

const QEvent::Type kLicenseFailEvent(static_cast<QEvent::Type>(QEvent::User + 1000));

class QCOMMONCONSOLE_EXPORT LicenseFailEvent :
	public QEvent
{
public:
	LicenseFailEvent(const QString& why);

	QString						_why;
};

#endif // LICENSEFAILEVENT_H
