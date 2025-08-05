#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

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
