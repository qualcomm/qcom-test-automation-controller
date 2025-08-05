#ifndef INTFILTEREDEDITOR_H
#define INTFILTEREDEDITOR_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include"QCommonGlobal.h"

// QCommon
class IntValidator;

// QT
#include <QLineEdit>

class QCOMMON_EXPORT IntFilteredEditor :
	public QLineEdit
{
	Q_OBJECT
public:
	IntFilteredEditor(QWidget* parent = Q_NULLPTR);
	~IntFilteredEditor();

	void addBannedInt(quint32 banMe);
	void removeBannedInt(quint32 unBanMe);

signals:

public slots:

private:
	IntValidator*				_validator{Q_NULLPTR};
};

#endif // INTFILTEREDEDITOR_H
