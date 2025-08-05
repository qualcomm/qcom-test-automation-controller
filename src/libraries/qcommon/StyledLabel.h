#ifndef STYLEDLABEL_H
#define STYLEDLABEL_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "QCommonGlobal.h"

#include <QColor>
#include <QFont>
#include <QWidget>

class QCOMMON_EXPORT StyledLabel :
	public QWidget
{
	Q_OBJECT
public:
	StyledLabel(QWidget* parent = Q_NULLPTR);

	void setColor(const QColor& foregroundColor);
	void setFont(const QFont& font);
	void setText(const QString& text);
	void setRotation(qreal rotation);

protected:
	virtual void paintEvent(QPaintEvent* paintEvt);

private:
	QString						_text{"foo"};
	QColor						_foregroundColor{Qt::black};
	QFont						_font{QFont("Arial", 10)};
	qreal						_rotation{0.0};
};

#endif // STYLEDLABEL_H
