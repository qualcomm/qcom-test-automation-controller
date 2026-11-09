#ifndef TACWINDOW_H
#define TACWINDOW_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

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

protected:
	void closeEvent(QCloseEvent* event) override;

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
	void on_actionSubmit_Bug_Report_triggered();

	// Finishes a disconnect (and a window close) that was requested while a
	// quick command was still running; see closeDevice().
	void onDeferredCloseDone();

private:
	AlpacaDevice				_alpacaDevice;
	QTimer						_autoShutdownTimer;
	QElapsedTimer				_autoShutdownDeadline;
	NotificationWidget*			_nwgt{Q_NULLPTR};

	void connect();
	void disconnect();

	// Stops the timers, detaches the frame and closes the device. Leaves the
	// window itself alone; disconnect() also resets the UI.
	//
	// Returns false when the close had to be deferred: Disconnect, window
	// close and auto shutdown can arrive from inside a running quick
	// command's wait loop, and tearing the frame (whose button emitted the
	// command) and the device down under it crashed TAC. The device then
	// finishes the close when the command returns and emits
	// deferredCloseDone, handled by onDeferredCloseDone().
	bool closeDevice();
	void showDisconnectedState();
	void updateConnectionControls();

	QString						_defaultTitle;

	void saveSettings();
	void restoreSettings();

	// Shows the connected board in the window title so several TAC windows
	// can be told apart. The board reports its hardware identity shortly
	// after connecting, so the title is refreshed until that arrives.
	void updateTitle();
	void onTitleRefreshTimeout();

	QTimer						_titleRefreshTimer;
	int							_titleRefreshAttempts{0};

	bool						_closeDeferred{false};
	bool						_closeWindowWhenDone{false};

    void setupLinuxFonts();
};

#endif // TACWINDOW_H
