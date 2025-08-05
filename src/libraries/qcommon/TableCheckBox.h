#ifndef TABLECHECKBOX_H
#define TABLECHECKBOX_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
		Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "QCommonGlobal.h"

// Qt
class QCheckBox;
#include <QWidget>

class QCOMMON_EXPORT TableCheckBox :
	public QWidget
{
Q_OBJECT

public:
	explicit TableCheckBox(QWidget* parent);

	Qt::CheckState checkState() const;

public slots:
	void setCheckState(bool state);
	void setCheckState(Qt::CheckState state);

signals:
	void checkStateChanged(bool newState);
	void checkStateChanged(Qt::CheckState newState);

private:
	QCheckBox*					_checkBox;
};

#endif // TABLECHECKBOX_H
