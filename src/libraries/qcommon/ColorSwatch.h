#ifndef COLORSWATCH_H
#define COLORSWATCH_H
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
// Copyright 2020-2021 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "QCommonGlobal.h"

#include <QColor>
#include <QWidget>

const quint32 kInvalidColorRow{0xFFFFFFFF};

class QCOMMON_EXPORT ColorSwatch :
	public QWidget
{
Q_OBJECT

public:
	ColorSwatch(QWidget* parent = Q_NULLPTR);

	QColor color()
	{
		return _color;
	}

	void setColor(QColor color)
	{
		_color = color;
		if (isVisible())
			update();
	}

	void setRow(quint32 row)
	{
		_row = row;
	}
	quint32 row()
	{
		return _row;
	}

signals:
	void editRequested(quint32 row);

protected:
	virtual void paintEvent(QPaintEvent* event);
	virtual void mouseDoubleClickEvent(QMouseEvent* mouseEvent);

private:
	QColor						_color{QColor::Invalid};
	quint32						_row{kInvalidColorRow};
};

#endif // COLORSWATCH_H
