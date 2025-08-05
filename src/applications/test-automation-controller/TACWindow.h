#ifndef TACWINDOW_H
#define TACWINDOW_H

/*
	Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries. 
	 
	Redistribution and use in source and binary forms, with or without
	modification, are permitted (subject to the limitations in the
	disclaimer below) provided that the following conditions are met:
	 
		* Redistributions of source code must retain the above copyright
		  notice, this list of conditions and the following disclaimer.
	 
		* Redistributions in binary form must reproduce the above
		  copyright notice, this list of conditions and the following
		  disclaimer in the documentation and/or other materials provided
		  with the distribution.
	 
		* Neither the name of Qualcomm Technologies, Inc. nor the names of its
		  contributors may be used to endorse or promote products derived
		  from this software without specific prior written permission.
	 
	NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
	GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
	HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
	WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
	ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
	GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
	IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
	IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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
