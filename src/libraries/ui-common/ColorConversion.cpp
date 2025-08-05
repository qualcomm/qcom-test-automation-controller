// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "ColorConversion.h"

const QColor kWarnNotice(0xff, 0xe0, 0xc0);
const QColor kInfoNotice(0x22, 0xbf, 0xe2);
const QColor kErrorNoticeQColor(0xfc, 0x5e, 0x03);


QColor ColorConversion::BasicToColor(const BasicColor &convertMe)
{
	QColor result;

	result.setRed(convertMe.red());
	result.setGreen(convertMe.green());
	result.setBlue(convertMe.blue());
	result.setAlpha(convertMe.alpha());

	return result;
}

BasicColor ColorConversion::ColorToBasic(const QColor &convertMe)
{
	BasicColor result(convertMe.red(), convertMe.green(), convertMe.blue(), convertMe.alpha());

	return result;
}

QColor ColorConversion::getLabelColor(const NotificationLevel level)
{
	QColor labelColor(Qt::lightGray);

	switch(level)
	{
	case eDebugNotification:
		break;
	case eInfoNotification:
		labelColor = kInfoNotice;
		break;
	case eWarnNotification:
		labelColor = kWarnNotice;
		break;
	case eErrorNotification:
		labelColor = kErrorNoticeQColor;
		break;
	}

	return labelColor;
}
