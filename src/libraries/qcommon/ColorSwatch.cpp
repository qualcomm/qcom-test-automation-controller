// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

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
