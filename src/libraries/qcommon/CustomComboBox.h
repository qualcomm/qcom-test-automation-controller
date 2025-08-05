#ifndef CUSTOMCOMBOBOX_H
#define CUSTOMCOMBOBOX_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "qevent.h"
#include <QComboBox>


class CustomComboBox : public QComboBox
{
	Q_OBJECT

public:
	CustomComboBox(QWidget* parent = Q_NULLPTR) :
		QComboBox(parent)
	{
		setFocusPolicy(Qt::ClickFocus);
	}

protected:
	virtual void wheelEvent(QWheelEvent* event)
	{
		if (!hasFocus())
			event->ignore();
		else
			QComboBox::wheelEvent(event);
	}
};


#endif // CUSTOMCOMBOBOX_H
