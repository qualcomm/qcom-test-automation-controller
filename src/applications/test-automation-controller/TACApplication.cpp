// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "TACApplication.h"

// TAC
#include "TACWindow.h"

#include "TACDefines.h"

// QCommon
#include "AboutDialog.h"
#include "AlpacaSettings.h"
#include "QuitAppEvent.h"

// QT
#include <QFile>
#include <QMessageBox>
#include <QTimer>

bool TACApplication::_starting{true};
QList<TACWindow*> TACApplication::_tacWindows;
QPoint TACApplication::_createWindowOrigin(40, 40);

TACApplication::TACApplication
(
	int& argc,
	char** argv
) :
	AlpacaApplication (argc, argv, kAppName, kAppVersion)
{
	_preferences.setAppName(kAppName.toLatin1(), kAppVersion.toLatin1());
	initialize(&_preferences);
}

TACApplication::~TACApplication()
{

}

TACApplication* TACApplication::tacAppInstance()
{
	return qobject_cast<TACApplication*>(QCoreApplication::instance());
}

TACWindow* TACApplication::createTACWindow
(
	bool show
)
{
	TACWindow* result;

	result = new TACWindow;

	if (result != Q_NULLPTR)
	{
		result ->move(_createWindowOrigin);
		_createWindowOrigin += QPoint(20, 20);

		if (show)
			result->show();

		_tacWindows.append(result);
	}
	else
	{
		TACApplication* app = tacAppInstance();
		if (app)
			app->appCore()->writeToAppLog("TACApplication::createTACWindow failed");
	}

	return result;
}

TACWindow* TACApplication::openByName
(
	const QByteArray& portName
)
{
	TACWindow* result{Q_NULLPTR};

	AlpacaDevice tacDevice = _AlpacaDevice::findAlpacaDevice(portName);

	QByteArray actualPortName = tacDevice->portName();
	if (actualPortName.isEmpty() == false)
	{
		for (const auto& tacWindow: std::as_const(_tacWindows))
		{
			if (tacWindow->portName().compare(portName, Qt::CaseInsensitive) == 0)
			{
				result = tacWindow;
				break;
			}
		}

		if (result == Q_NULLPTR)
		{
			result = createTACWindow(false);
			if (result != Q_NULLPTR)
			{
				result->openPort(portName);
			}
		}
	}
	
	return result;
}

void TACApplication::showAboutDialog()
{
	AboutDialog aboutDialog(Q_NULLPTR);

	aboutDialog.setTitle("<html><head/><body><p><span style=\" font-size:12pt; font-weight:600;\">Test Automation Controller</span></p></body></html>");

	QFile file(":/About.txt");
	if (file.open(QIODevice::ReadOnly) == true)
	{
		QByteArray html = file.readAll();
		aboutDialog.setAboutText(html);
		file.close();
	}

	aboutDialog.setAppName(kAppName);
	aboutDialog.setAppVersion(kAppVersion.toLatin1());
	QPixmap pixMap = QPixmap(QString::fromUtf8(":/TAC.png"));
	aboutDialog.setBackSplash(pixMap);

	aboutDialog.exec();
}

void TACApplication::quitTAC()
{
	shutDown();
}

void TACApplication::quitTACByMsg()
{
	AlpacaSettings settings(kAppName);

	postEvent(this, new QuitAppEvent);
}

void TACApplication::on_preferencesChanged()
{
	emit preferencesChanged();
}

void TACApplication::shutDown()
{
	// Work on a copy: a window may leave the list while it shuts down.
	const QList<TACWindow*> tacWindows{_tacWindows};
	_tacWindows.clear();

	for (TACWindow* tacWindow: tacWindows)
		tacWindow->shutDown();

	AppCore::getAppCore()->setAppLogging(false);
	quit();
}

bool TACApplication::disconnectTACWindow
(
	TACWindow *tacWindow
)
{
	bool result{false};

	int indexof = _tacWindows.indexOf(tacWindow);
	if (indexof != -1)
	{
		_tacWindows.removeAt(indexof);
		result = true;
	}

	// Only removes the window from the list. Disconnecting a device must not
	// end the application; closing the last window does (TACWindow::closeEvent).
	return result;
}

int TACApplication::windowCount()
{
	return _tacWindows.count();
}

bool TACApplication::openLastDevice()
{
	bool result{false};

	if (_starting)
	{
		_starting = false;

		if (_preferences.openLastStart())
		{
			// Older versions saved the port with a leading space (" COM7").
			QByteArray lastDevice = _preferences.lastDevice().trimmed().toLatin1();
			if (lastDevice.isEmpty() == false)
			{
				// Nothing has enumerated the boards yet at startup; without this
				// the search below always fails.
				_AlpacaDevice::updateAlpacaDevices();

				AlpacaDevice alpacaDevice = _AlpacaDevice::findAlpacaDevice(lastDevice);
				if (alpacaDevice.isNull())
				{
					// The board may simply be unplugged today, so keep the
					// preference and the saved device for the next start.
					AppCore::writeToApplicationLogLine(QString("LastDevice %1 not found").arg(QString(lastDevice)));
				}
				else
				{
					AppCore::writeToApplicationLogLine(QString("LastDevice: %1").arg(QString(lastDevice)));

					TACWindow* tacWindow = openByName(lastDevice);
					if (tacWindow != Q_NULLPTR && tacWindow->inUse())
					{
						tacWindow->show();

						result = true;
					}
					else if (tacWindow != Q_NULLPTR)
					{
						// The board was found but could not be opened; main()
						// creates the normal empty window instead.
						disconnectTACWindow(tacWindow);
						tacWindow->deleteLater();
					}
				}
			}
		}
	}

	return result;
}

bool TACApplication::checkToSeeIfPortIsInUse
(
	const QByteArray& portName
)
{
	bool result{false};

	for (const auto& tacWindow: std::as_const(_tacWindows))
	{
		if (tacWindow->portName() == portName)
		{
			result = true;
			break;
		}
	}

	return result;
}
