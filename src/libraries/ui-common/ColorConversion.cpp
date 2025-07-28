// Confidential and Proprietary – Qualcomm Technologies, Inc.

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
// Copyright ©2023-2025 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
