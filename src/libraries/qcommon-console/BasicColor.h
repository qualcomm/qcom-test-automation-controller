#ifndef BASICCOLOR_H
#define BASICCOLOR_H
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

// QT's color class is based in the gui lib.  We want this library to not be dependant on gui or widgets.

#include <QtGlobal>

const quint16 kOpaque{255};

#include <QString>

class BasicColor
{

public:
	BasicColor() = default;
	BasicColor(quint16 red, quint16 green, quint16 blue, quint16 alpha = kOpaque);
	BasicColor(const BasicColor& copyMe) = default;
	BasicColor& operator= (const BasicColor& assignMe) = default;

	bool isValid()
	{
		return _valid;
	}

	quint16 red() const { return _red; }
	quint16 green() const { return _green; }
	quint16 blue() const { return _blue; }
	quint16 alpha() const { return _alpha; }

	QString name() const;
	void setNamedColor(const QString& namedColor);

	BasicColor lighter(quint16 scale);

private:
	bool						_valid{false};
	quint16						_alpha{kOpaque};
	quint16						_red{0};
	quint16						_green{0};
	quint16						_blue{0};
};

#endif // BASICCOLOR_H
