#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H
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
// Copyright 2019 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "QCommonGlobal.h"

class Ui_ProgressDialogClass;

// Qt
#include <QDialog>
#include <QTimer>

class QCOMMON_EXPORT ProgressDialog :
	public QDialog
{
Q_OBJECT

public:
	ProgressDialog(const QString& description, quint32 duration, QWidget* parent = Q_NULLPTR);

private slots:
	void on__timer_timeout();

protected:
	void changeEvent(QEvent* e);

	Ui_ProgressDialogClass*		_ui{Q_NULLPTR};

	QTimer						_timer;
	quint32						_current;
	quint32						_duration;
};

#endif // PROGRESSDIALOG_H
