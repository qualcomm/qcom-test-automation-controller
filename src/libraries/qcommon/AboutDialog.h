#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H
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
// Copyright 2018-2021 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// QCommon
#include "QCommonGlobal.h"

// QT
#include <QDialog>
#include <QPixmap>

namespace Ui
{
	class AboutDialogClass;
}

class QCOMMON_EXPORT AboutDialog :
	public QDialog
{
Q_OBJECT

public:
	AboutDialog(QWidget* parent = Q_NULLPTR);

	void setTitle(const QByteArray& title);
	void setAppName(const QString& appName);
	void setAppVersion(const QByteArray& appVersion);
	void setAboutText(const QByteArray& aboutText);
	void setBackSplash(const QPixmap& backSplash);

private slots:
	void on__aboutText_anchorClicked(const QUrl& link);
	void on__checkDateButton_clicked();
	void on__okayButton_clicked();

protected:
	virtual void changeEvent(QEvent* e);
	virtual void mouseReleaseEvent(QMouseEvent* mouseReleaseEvent);

private:
	Ui::AboutDialogClass*		_ui{Q_NULLPTR};
};

#endif // ABOUTDIALOG_H
