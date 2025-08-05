#ifndef COLORCONVERSION_H
#define COLORCONVERSION_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "QCommonGlobal.h"
#include "BasicColor.h"

#include "Notification.h"

#include <QColor>

class QCOMMON_EXPORT ColorConversion
{
public:
	static QColor BasicToColor(const BasicColor& convertMe);
	static BasicColor ColorToBasic(const QColor& convertMe);
	static QColor getLabelColor(const NotificationLevel level);
};

#endif // COLORCONVERSION_H
