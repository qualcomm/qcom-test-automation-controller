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
// Copyright 2023 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "BasicColor.h"

BasicColor::BasicColor
(
	quint16 red,
	quint16 green,
	quint16 blue,
	quint16 alpha
)
{
	_valid = true;
	_red = red;
	_green = green;
	_blue = blue;
	_alpha = alpha;
}

QString BasicColor::name() const
{
	QString result("#");

    result +=  QString("%1").arg(_red, 2, 16, QLatin1Char('0'));
    result +=  QString("%1").arg(_green, 2, 16, QLatin1Char('0'));
    result +=  QString("%1").arg(_blue, 2, 16, QLatin1Char('0'));

	return  result;
}

void BasicColor::setNamedColor
(
	const QString& namedColor
)
{
	_valid = false;

	if (namedColor[0] == '#')
	{
		if (namedColor.length() >= 7)
		{
			QString redString = namedColor.mid(1, 2);
			QString greenString = namedColor.mid(3, 2);
			QString blueString = namedColor.mid(5, 2);

			_red = redString.toUShort(Q_NULLPTR, 16);
			_green = greenString.toUShort(Q_NULLPTR, 16);
			_blue = blueString.toUShort(Q_NULLPTR, 16);

			_valid = true;
		}
	}
}

BasicColor BasicColor::lighter(quint16 scale)
{
	BasicColor result(_red, _green, _blue);

	if (scale > 100 && scale < 151)
	{
		qreal factor = scale / 100.0;
		if (factor)
		{
			result._red = _red * factor;
			if (result.red() > 255)
				result._red = 255;

			result._green = _green * factor;
			if (result.green() > 255)
				result._green = 255;

			result._blue = _blue * factor;
			if (result.blue() > 255)
				result._blue = 255;

			result._valid = true;
		}
	}

	return result;
}
