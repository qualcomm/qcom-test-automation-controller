#ifndef HAPPINESSDIALOG_H
#define HAPPINESSDIALOG_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
    Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

//QCommon
#include "QCommonGlobal.h"

//QT
#include <QDialog>
#include <QLabel>
#include <QPixmap>
#include <QMouseEvent>

namespace Ui {
class HappinessDialogClass;
}

enum HappinessRating
{
	eNotSet,
	eDissatisfied,
	eUnhappy,
	eSatisfied,
	eHappy,
	eLove
};

class QCOMMON_EXPORT HappinessDialog :
    public QDialog
{
	Q_OBJECT

public:
    explicit HappinessDialog(QWidget* parent = Q_NULLPTR);
	~HappinessDialog();

	HappinessRating getRating();

	static QString ratingToString(HappinessRating rating);

protected:
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
private:
    Ui::HappinessDialogClass*   _ui{Q_NULLPTR};

	bool                          _responseChosen{false};
    QLabel*                       _selectedLabel{Q_NULLPTR};
    QLabel*                       _activeLabel{Q_NULLPTR};
    QPoint                        _lastPosition;
    QSize                         _lastSize;
};

#endif // HAPPINESSDIALOG_H
