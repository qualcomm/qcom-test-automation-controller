#ifndef HAPPINESSDIALOG_H
#define HAPPINESSDIALOG_H

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
// Copyright 2018-2020 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
