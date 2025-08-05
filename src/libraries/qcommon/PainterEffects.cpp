/*
	Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries. 
	 
	Redistribution and use in source and binary forms, with or without
	modification, are permitted (subject to the limitations in the
	disclaimer below) provided that the following conditions are met:
	 
		* Redistributions of source code must retain the above copyright
		  notice, this list of conditions and the following disclaimer.
	 
		* Redistributions in binary form must reproduce the above
		  copyright notice, this list of conditions and the following
		  disclaimer in the documentation and/or other materials provided
		  with the distribution.
	 
		* Neither the name of Qualcomm Technologies, Inc. nor the names of its
		  contributors may be used to endorse or promote products derived
		  from this software without specific prior written permission.
	 
	NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
	GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
	HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
	WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
	ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
	GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
	IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
	IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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

