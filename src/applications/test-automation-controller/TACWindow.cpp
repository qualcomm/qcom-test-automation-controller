// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

//

#include "TACWindow.h"

// TAC
#include "TACApplication.h"
#include "TACDeviceSelection.h"
#include "PreferencesDialog.h"

#include "TACDefines.h"
#include "TACPreferences.h"

// QCommon
#include "AlpacaDefines.h"
#include "AlpacaSettings.h"
#include "ApplicationEnhancements.h"
#include "ConsoleApplicationEnhancements.h"

// QT
#include <QCloseEvent>
#include <QDir>
#include <QFileInfoList>
#include <QInputDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QScrollBar>

TACWindow::TACWindow
(
	QWidget* parent
) :
	QMainWindow(parent)
{
	setupUi(this);

	// Only distributions that ship BugWriter offer bug reporting.
	actionSubmit_Bug_Report->setVisible(bugWriterAvailable());

	QString windowTitle{this->windowTitle()};
	windowTitle = QString(windowTitle).arg("QTAC");
	this->setWindowTitle(windowTitle);

	// Restored when the device is disconnected.
	_defaultTitle = windowTitle;

	_nwgt = new NotificationWidget(_statusbar);
	_statusbar->addPermanentWidget(_nwgt);

	restoreSettings();

	QObject::connect(&_autoShutdownTimer, &QTimer::timeout, this, &TACWindow::onAutoShutdownTimerTimeout);

	_titleRefreshTimer.setInterval(500);
	QObject::connect(&_titleRefreshTimer, &QTimer::timeout, this, &TACWindow::onTitleRefreshTimeout);
	QObject::connect(this, &TACWindow::progress, _nwgt, &NotificationWidget::progress, Qt::DirectConnection);

	updateConnectionControls();
}

TACWindow::~TACWindow()
{
	saveSettings();

	closeDevice();
	TACApplication::disconnectTACWindow(this);
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

namespace
{

const int kTitleRefreshMaxAttempts{20};		// 20 x 500 ms = 10 seconds
const int kSerialFullLength{12};
const int kSerialTailLength{8};

// Long serial numbers often share a common prefix across boards of the same
// type, so the end is the part that tells them apart.
QString shortSerial(const QString& serialNumber)
{
	if (serialNumber.size() <= kSerialFullLength)
		return serialNumber;

	return QString(QChar(0x2026)) + serialNumber.right(kSerialTailLength);
}

// The hardware description reads "Unknown Board ID: n" until the board has
// reported its identity, which is not useful in a title.
bool isKnownHardware(const QString& hardware)
{
	return hardware.isEmpty() == false &&
		   hardware.startsWith(QStringLiteral("Unknown Board ID"), Qt::CaseInsensitive) == false;
}

} // namespace

void TACWindow::updateTitle()
{
	if (_alpacaDevice.isNull())
		return;

	const QString separator = QStringLiteral(" ") + QChar(0x00B7) + QStringLiteral(" ");

	QStringList parts;
	parts << QString::fromLatin1(_alpacaDevice->portName());

	const QString serialNumber = QString::fromLatin1(_alpacaDevice->serialNumber()).trimmed();
	if (serialNumber.isEmpty() == false)
		parts << QStringLiteral("SN ") + shortSerial(serialNumber);

	const QString hardware = _alpacaDevice->hardwareVersionString().trimmed();
	if (isKnownHardware(hardware))
		parts << QStringLiteral("HW ") + hardware;

	setWindowTitle(parts.join(separator));
}

void TACWindow::onTitleRefreshTimeout()
{
	updateTitle();

	_titleRefreshAttempts++;

	const bool hardwareKnown = _alpacaDevice.isNull() == false &&
							   isKnownHardware(_alpacaDevice->hardwareVersionString().trimmed());

	if (hardwareKnown || _alpacaDevice.isNull() || _titleRefreshAttempts >= kTitleRefreshMaxAttempts)
		_titleRefreshTimer.stop();
}

void TACWindow::closeDevice()
{
	_titleRefreshTimer.stop();
	_autoShutdownTimer.stop();

	if (_alpacaDevice.isNull() == false)
	{
		QObject::disconnect(_alpacaDevice.data(), Q_NULLPTR, _nwgt, Q_NULLPTR);

		_tacFrame->setDevice(AlpacaDevice(Q_NULLPTR));
		_alpacaDevice->close();
		_alpacaDevice.reset();
	}
}

void TACWindow::showDisconnectedState()
{
	_deviceStatusLabel->setText("Disconnected");
	setWindowTitle(_defaultTitle);
	_statusbar->clearMessage();

	updateConnectionControls();
}

void TACWindow::updateConnectionControls()
{
	const bool connected = inUse();

	_disconnectButton->setEnabled(connected);
	actionDisconnect->setEnabled(connected);
}

// Called by TACApplication when the application quits. The application owns
// the window list at that point, so the window must not remove itself.
void TACWindow::shutDown()
{
	closeDevice();
}

void TACWindow::closeEvent(QCloseEvent* event)
{
	closeDevice();
	TACApplication::disconnectTACWindow(this);

	event->accept();
	deleteLater();

	// Closing the last window still ends the application.
	if (TACApplication::windowCount() == 0)
		TACApplication::tacAppInstance()->quitTAC();
}

void TACWindow::onAutoShutdownTimerTimeout()
{
	qint64 elapsed = _autoShutdownDeadline.elapsed();
	if (elapsed > TACApplication::tacAppInstance()->preferences()->autoShutdownDelayInMSecs())
	{
		if (inUse())
		{
			disconnect();
			_statusbar->showMessage("Disconnected by auto shutdown");
		}
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
	emit progress();

	AlpacaDevice alpacaDevice = _AlpacaDevice::findAlpacaDevice(portName);
	if (alpacaDevice.isNull())
	{
		emit progress(90, eErrorNotification);
		_statusbar->showMessage("Device " + QString::fromLatin1(portName.trimmed()) + " was not found");
		return;
	}

	if (alpacaDevice->open() == false)
	{
		// Leave the window disconnected so it can be used for another device.
		emit progress(90, eErrorNotification);
		_statusbar->showMessage("Unable to open " + QString::fromLatin1(alpacaDevice->portName()));
		return;
	}

	_alpacaDevice = alpacaDevice;

	_tacFrame->setDevice(_alpacaDevice);

	// The hardware identity arrives shortly after the port opens, so
	// show what is known now and refresh for a few seconds.
	updateTitle();
	_titleRefreshAttempts = 0;
	_titleRefreshTimer.start();

	_deviceStatusLabel->setText("Connected");
	updateConnectionControls();

	resize(_alpacaDevice->windowDimension());

	const QByteArray lastError = _alpacaDevice->getLastError();
	if (lastError.isEmpty() == false)
		_statusbar->showMessage("Error: " + lastError);

	// Remember the board by serial number when it has one: COM port numbers
	// change between sessions, the serial number does not. findAlpacaDevice()
	// accepts either.
	QByteArray lastDevice = _alpacaDevice->serialNumber().trimmed();
	if (lastDevice.isEmpty())
		lastDevice = _alpacaDevice->portName().trimmed();
	TACApplication::tacAppInstance()->preferences()->saveLastDevice(QString::fromLatin1(lastDevice));

	QObject::connect(_alpacaDevice.data(), &_AlpacaDevice::progress, _nwgt, &NotificationWidget::progress);

	setupAutoShutdownTimer();

	emit progress(80);
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

// Disconnecting only releases the device: the window stays open, in the
// application's window list, and looks the way it did before connecting,
// so Connect can be used again straight away.
void TACWindow::disconnect()
{
	closeDevice();
	showDisconnectedState();
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
	QStringList arguments;
	arguments << "product:QTAC";
	arguments << "prodversion:" + kProductVersion;
	arguments << "application:Test_Automation_Controller";
	arguments << "appversion:" + kAppVersion;
	arguments << "listports:";

	startBugWriter(arguments, this);
}
