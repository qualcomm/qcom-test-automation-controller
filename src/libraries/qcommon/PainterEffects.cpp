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
	Author: Austin Simpson (austinsimpson.dev)
*/

#include "PainterEffects.h"

#include <QFontMetricsF>
#include <QPainterPath>

void paintCenterRotatedText
(
	QPainter& painter,
	const QPointF& center,
	const QString& text,
	qreal theta
)
{
	if (painter.isActive())
	{
		QFontMetricsF fontMetrics(painter.font());
		painter.save();
		painter.setRenderHint(QPainter::Antialiasing, true); //Antialiased text is pretty
		painter.setBrush(Qt::SolidPattern); //Make the brush that we're working with solid, so that it fills in the characters
		painter.setPen(Qt::NoPen); //Text will appear to be larger than desired.

		painter.translate(center); //Translate to the spot that we want to draw the text at
		painter.rotate(-theta); //Rotate to our target angle

		QPainterPath textPath; //Create a painter path that will do some magic for us
		textPath.addText(QPointF(-fontMetrics.horizontalAdvance(text) / 2, fontMetrics.height() / 2), painter.font(), text); //Add our text to the path, centered on the desired point.
		painter.drawPath(textPath); //Then, we draw the path

		painter.restore();
	}
}

void paintLeftRotatedText
(
	QPainter& painter,
	const QPointF& anchor,
	const QString& text,
	qreal theta
)
{
	if (painter.isActive())
	{
		QFontMetricsF fontMetrics(painter.font());
		painter.save();
		painter.setRenderHint(QPainter::Antialiasing, true); //Antialiased text is pretty
		painter.setBrush(Qt::SolidPattern); //Make the brush that we're working with solid, so that it fills in the characters
		painter.setPen(Qt::NoPen); //Text will appear to be larger than desired.

		painter.translate(anchor); //Translate to the spot that we want to draw the text at
		painter.rotate(theta); //Rotate to our target angle

		QPainterPath textPath; //Create a painter path that will do some magic for us
		textPath.addText(QPointF(0, fontMetrics.capHeight() / 2), painter.font(), text); //Add our text to the path, centered on the desired point.
		painter.drawPath(textPath); //Then, we draw the path

		painter.restore();
	}
}

void paintRightRotatedText
(
	QPainter& painter,
	const QPointF& anchor,
	const QString& text,
	qreal theta
)
{
	if (painter.isActive())
	{
		QFontMetricsF fontMetrics(painter.font());
		painter.save();
		painter.setRenderHint(QPainter::Antialiasing, true); //Antialiased text is pretty
		painter.setBrush(Qt::SolidPattern); //Make the brush that we're working with solid, so that it fills in the characters
		painter.setPen(Qt::NoPen); //Text will appear to be larger than desired.

		painter.translate(anchor); //Translate to the spot that we want to draw the text at
		painter.rotate(theta); //Rotate to our target angle

		QPainterPath textPath; //Create a painter path that will do some magic for us
		textPath.addText(QPointF(-fontMetrics.horizontalAdvance(text), fontMetrics.capHeight() / 2), painter.font(), text); //Add our text to the path, centered on the desired point.
		painter.drawPath(textPath); //Then, we draw the path

		painter.restore();
	}
}

