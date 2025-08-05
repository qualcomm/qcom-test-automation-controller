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

// Author: Michael Simpson msimpson@qti.qualcomm.com
//	Biswajit Roy biswroy@qti.qualcomm.com
//

#include "TACWindow.h"

// TAC
#include "TACApplication.h"
#include "TACDeviceSelection.h"
#include "PreferencesDialog.h"

// libTAC
#include "TACDefines.h"
#include "TACPreferences.h"

// QCommon
#include "AlpacaDefines.h"
#include "AlpacaSettings.h"
#include "ApplicationEnhancements.h"

// QT
#include <QDir>
#include <QFileInfoList>
#include <QInputDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QProcess>
#include <QScrollBar>

const QString kWindowTitle(QStringLiteral("Test Automation Controller %1"));

TACWindow::TACWindow
(
	QWidget* parent
) :
	QMainWindow(parent)
{
	setupUi(this);

	QString windowTitle{this->windowTitle()};
	windowTitle = QString(windowTitle).arg("QTAC");
	this->setWindowTitle(windowTitle);

	_nwgt = new NotificationWidget(_statusbar);
	_statusbar->addPermanentWidget(_nwgt);

	restoreSettings();

	QObject::connect(&_autoShutdownTimer, &QTimer::timeout, this, &TACWindow::onAutoShutdownTimerTimeout);
	QObject::connect(this, &TACWindow::progress, _nwgt, &NotificationWidget::progress, Qt::DirectConnection);
}

TACWindow::~TACWindow()
{
	saveSettings();

	disconnect();
}

void TACWindow::saveSettings()
{
	AlpacaSettings settings(kAppName);
	settings.beginGroup("TACWindow");
	settings.setValue("size", size());
	settings.endGroup();
}

void TACWindow::restoreSettings()
{
	AlpacaSettings settings(kAppName);
	settings.beginGroup("TACWindow");

	QSize size = settings.value("size", QSize(555, 725)).toSize();
	if (size.width() < 555)
		size.setWidth(555);
	if (size.height() < 725)
		size.setHeight(725);
	resize(size);

	settings.endGroup();
}

void TACWindow::shutDown()
{
	if (_alpacaDevice.isNull() == false)
	{
		TACApplication::disconnectTACWindow(this);

		_tacFrame->setDevice(AlpacaDevice(Q_NULLPTR));
		_alpacaDevice->close();
		_alpacaDevice = AlpacaDevice(Q_NULLPTR);
	}
}

void TACWindow::onAutoShutdownTimerTimeout()
{
	qint64 elapsed = _autoShutdownDeadline.elapsed();
	if (elapsed > TACApplication::tacAppInstance()->preferences()->autoShutdownDelayInMSecs())
	{
		shutDown();
	}
}

void TACWindow::setupAutoShutdownTimer()
{
	if (TACApplication::tacAppInstance()->preferences()->autoShutdownActive())
	{
		_autoShutdownTimer.start(60000); // 60 seconds
		_autoShutdownDeadline.restart();
	}
	else
	{
		_autoShutdownTimer.stop();
	}
}

void TACWindow::on_actionPreferences_triggered()
{
	TACPreferences* preferences = TACApplication::tacAppInstance()->preferences();
	PreferencesDialog dialog(preferences, this);

	if (dialog.exec() == QDialog::Accepted)
	{
		bool loggingEnabled = preferences->loggingActive();

		AppCore::getAppCore()->setAppLogging(loggingEnabled);
		AppCore::getAppCore()->setRunLogging(loggingEnabled);

		setupAutoShutdownTimer();
	}
}

QByteArray TACWindow::latestLogFile()
{
	QByteArray result;

	QString logFilePath = TACApplication::tacAppInstance()->preferences()->runLogPath();
	QDir dir(logFilePath);

	dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
	dir.setSorting(QDir::Time | QDir::Reversed);

	QFileInfoList list = dir.entryInfoList();
	if (list.isEmpty() == false)
	{
		result = list.at(0).absoluteFilePath().toLatin1();
	}

	return result;
}

void TACWindow::openPort
(
	const QByteArray &portName
)
{
	_alpacaDevice = _AlpacaDevice::findAlpacaDevice(portName);

	emit progress();

	if (_alpacaDevice.isNull() == false)
	{
		if (_alpacaDevice->open() == true)
		{
			int minorVersion = _alpacaDevice->minorVersion();


			_tacFrame->setDevice(_alpacaDevice);

			QString portName = " " + _alpacaDevice->portName();
			setWindowTitle(QString(kWindowTitle).arg(portName));

			_disconnectButton->setEnabled(true);

			_deviceStatusLabel->setText("Connected");

			resize(_alpacaDevice->windowDimension());

			if (_alpacaDevice->getLastError().isEmpty() == false)
				_statusbar->showMessage("Error: " + _alpacaDevice->getLastError());

			TACApplication::tacAppInstance()->preferences()->saveLastDevice(portName);

			QObject::connect(_alpacaDevice.data(), &_AlpacaDevice::progress, _nwgt, &NotificationWidget::progress);
			emit progress(80);
		}
		else
		{
			// do nothing
		}
	}
}

QByteArray TACWindow::portName() const
{
	if (_alpacaDevice.isNull() == false)
		return _alpacaDevice->portName();

	return QByteArray();
}

void TACWindow::on_actionQuit_triggered()
{
	TACApplication::tacAppInstance()->quitTAC();
}

void TACWindow::on_actionExit_triggered()
{
	TACApplication::tacAppInstance()->quitTAC();
}

void TACWindow::connect()
{
	TACDeviceSelection deviceSelection(Q_NULLPTR);

	if (deviceSelection.exec() == QDialog::Accepted)
	{
		QByteArray selectedSerialPort = deviceSelection.getSelectedPortName();

		emit progress();

		if (TACApplication::checkToSeeIfPortIsInUse(selectedSerialPort))
		{
			emit progress(90, eErrorNotification);

			QMessageBox::critical(this, "Port In Use",
				"Port " + selectedSerialPort + " is already in use."
			);

			return;
		}

		if (selectedSerialPort.isEmpty() == false)
		{
			if (inUse() == false)
			{
				openPort(selectedSerialPort);
			}
			else
			{
				TACWindow* tacWindow = TACApplication::createTACWindow();
				if (tacWindow != Q_NULLPTR)
				{
					tacWindow->openPort(selectedSerialPort);
				}
			}
		}
	}
}

void TACWindow::on_actionConnect_triggered()
{
	connect();
}
void TACWindow::on__connectButton_clicked()
{
	connect();
}

void TACWindow::on__disconnectButton_clicked()
{
	disconnect();
}

void TACWindow::on_actionDisconnect_triggered()
{
	disconnect();
}

void TACWindow::disconnect()
{
	shutDown();

	_autoShutdownTimer.stop();
	TACApplication::disconnectTACWindow(this);

	deleteLater();
}

void TACWindow::on_actionContents_triggered()
{
	startLocalBrowser(docsRoot() + "/getting-started/03-Test-Automation-Controller.html");
}

void TACWindow::on_actionAbout_triggered()
{
	TACApplication::tacAppInstance()->showAboutDialog();
}

void TACWindow::on_actionRate_Me_triggered()
{
	TACApplication::tacAppInstance()->showRateDialog();
}

void TACWindow::on_actionSubmit_Bug_Report_triggered()
{
#ifdef Q_OS_LINUX
	QString program = "/opt/qcom/QTAC/bin/BugWriter"; // Linux Sucks
#else
	QString program = "BugWriter";
#endif

	QStringList arguments;
	arguments << "product:QTAC";
	arguments << "prodversion:" + kProductVersion;
	arguments << "application:Test_Automation_Controller";
	arguments << "appversion:" + kAppVersion;
	arguments << "listports:";

	QProcess* process = new QProcess(Q_NULLPTR);

	process->setProgram(program);
	process->setArguments(arguments);
	process->startDetached();
}

