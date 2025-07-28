#ifndef TACWINDOW_H
#define TACWINDOW_H

// Confidential and Proprietary – Qualcomm Technologies, Inc.

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
// Copyright ©2018-2025 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

// Author:	Michael Simpson <msimpson@qti.qualcomm.com>
//			Biswajit Roy <biswroy@qti.qualcomm.com>

#include "ui_TACWindow.h"
#include "NotificationWidget.h"

// Qt
#include <QElapsedTimer>
#include <QTimer>

class TACWindow :
	public QMainWindow,
	private Ui::TACWindowClass
{
Q_OBJECT

public:
	TACWindow(QWidget* parent = Q_NULLPTR);
	~TACWindow();

	bool inUse()
	{
		return _alpacaDevice.isNull() == false;
	}

	void openPort(const QByteArray& portName);
	QByteArray portName() const;
	void shutDown();

	QByteArray latestLogFile();

signals:
	void progress(const quint8 value=10, NotificationLevel level=eInfoNotification);

private slots:
	void onAutoShutdownTimerTimeout();
	void setupAutoShutdownTimer();

	void on_actionPreferences_triggered();
	void on_actionQuit_triggered();
	void on_actionExit_triggered();

	void on_actionConnect_triggered();
	void on__connectButton_clicked();

	void on_actionDisconnect_triggered();
	void on__disconnectButton_clicked();

	void on_actionContents_triggered();
	void on_actionAbout_triggered();
	void on_actionRate_Me_triggered();
	void on_actionSubmit_Bug_Report_triggered();


private:
	AlpacaDevice				_alpacaDevice;
	QTimer						_autoShutdownTimer;
	QElapsedTimer				_autoShutdownDeadline;
	NotificationWidget*			_nwgt{Q_NULLPTR};

	void connect();
	void disconnect();

	void saveSettings();
	void restoreSettings();

    void setupLinuxFonts();
};

#endif // TACWINDOW_H
