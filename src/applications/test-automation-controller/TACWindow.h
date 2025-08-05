#ifndef TACWINDOW_H
#define TACWINDOW_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

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
