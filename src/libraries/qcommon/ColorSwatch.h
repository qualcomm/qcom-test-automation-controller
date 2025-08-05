#ifndef COLORSWATCH_H
#define COLORSWATCH_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

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
