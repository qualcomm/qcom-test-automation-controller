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
// Copyright 2020 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "ColorSwatch.h"

// Qt
#include <QPainter>

ColorSwatch::ColorSwatch
(
	QWidget* parent
) : QWidget(parent)
{
	//QSizePolicy sizePolicy;

	//sizePolicy.setVerticalPolicy(QSizePolicy::Fixed);
	//sizePolicy.setHorizontalPolicy(QSizePolicy::Fixed);

	//setSizePolicy(sizePolicy);
}

void ColorSwatch::paintEvent
(
	QPaintEvent* event
)
{
	Q_UNUSED(event)

	QPainter painter(this);

	QRect bounds = rect();

	bounds.adjust(1, 1, -1, -1);

	painter.fillRect(bounds, QBrush(_color));
}

void ColorSwatch::mouseDoubleClickEvent
(
	QMouseEvent* mouseEvent
)
{
	Q_UNUSED(mouseEvent)

	emit editRequested(_row);
}
