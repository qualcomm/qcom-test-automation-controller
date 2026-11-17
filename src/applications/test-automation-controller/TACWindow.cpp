// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

//

#include "TACWindow.h"

// TAC
#include "TACApplication.h"
#include "TACDeviceSelection.h"
#include "PreferencesDialog.h"

#include "QTACDefines.h"
#include "QTACPreferences.h"

// QCommon
#include "QAlpacaSettings.h"
#include "ApplicationEnhancements.h"
#include "QCommonTypes.h"
#include "QAlpacaDevice.h"
#include "QAppPaths.h"

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
	QAlpacaSettings settings(kAppName);
	settings.beginGroup("TACWindow");
	settings.setValue("size", size());
	settings.endGroup();
}

void TACWindow::restoreSettings()
{
	QAlpacaSettings settings(kAppName);
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
	if (_alpacaBridge != nullptr)
	{
		TACApplication::disconnectTACWindow(this);

		_tacFrame->setDevice(nullptr);
		_alpacaBridge->close();
		delete _alpacaBridge;
		_alpacaBridge = nullptr;
		_alpacaDevice = nullptr;
	}
	else if (_alpacaDevice != nullptr)
	{
		TACApplication::disconnectTACWindow(this);

		_tacFrame->setDevice(nullptr);
		_alpacaDevice->close();
		_alpacaDevice = nullptr;
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
	QTACPreferences* preferences = TACApplication::tacAppInstance()->preferences();
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
	_alpacaDevice = QAlpacaDevice::findAlpacaDevice(toQString(portName));

	emit progress();

	if (_alpacaDevice != nullptr)
	{
		if (_alpacaBridge != nullptr)
		{
			delete _alpacaBridge;
		}
		_alpacaBridge = new QAlpacaDevice(_alpacaDevice, this);

		if (_alpacaBridge->open() == true)
		{
			int minorVersion = _alpacaDevice->minorVersion();

			_tacFrame->setDevice(_alpacaDevice);

			QString portNameStr = " " + _alpacaBridge->portName();
			setWindowTitle(QString(kWindowTitle).arg(portNameStr));

			_disconnectButton->setEnabled(true);

			_deviceStatusLabel->setText("Connected");

			resize(_alpacaBridge->windowDimension());

			if (_alpacaBridge->getLastError().isEmpty() == false)
				_statusbar->showMessage("Error: " + _alpacaBridge->getLastError());

			TACApplication::tacAppInstance()->preferences()->saveLastDevice(_alpacaBridge->portName());

			QObject::connect(_alpacaBridge, &QAlpacaDevice::progress, _nwgt, &NotificationWidget::progress);
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
	if (_alpacaBridge != nullptr)
		return _alpacaBridge->portName().toLatin1();
	if (_alpacaDevice != nullptr)
		return QByteArray::fromStdString(_alpacaDevice->portName());

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

void TACWindow::on_actionSubmit_Bug_Report_triggered()
{
	QString program = applicationBinPath() + "BugWriter";

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

