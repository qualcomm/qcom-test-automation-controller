// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "StyledLabel.h"

// QCommon
#include "PainterEffects.h"

//QT
#include <QPainter>

StyledLabel::StyledLabel
(
	QWidget* parent
) :
	QWidget(parent),
	_text{"Foo"}
{

}

void StyledLabel::setColor(const QColor& foregroundColor)
{
	_foregroundColor = foregroundColor;

	update();
}

void StyledLabel::setFont(const QFont& font)
{
	_font = font;

	update();
}

void StyledLabel::paintEvent
(
	QPaintEvent* paintEvt
)
{
	Q_UNUSED(paintEvt);

	QPainter painter(this);

	painter.save();
	painter.setPen(_foregroundColor);
	painter.setFont(_font);

	paintCenterRotatedText(painter, rect().center(), _text, _rotation);
	painter.restore();
}

void StyledLabel::setText(const QString& text)
{
	_text = text;

	update();
}

void StyledLabel::setRotation
(
	qreal rotation
)
{
	_rotation = rotation;

	update();
}
