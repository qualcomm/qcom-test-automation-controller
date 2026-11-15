// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "TACApplication.h"

// TAC
#include "TACWindow.h"

// QCommon
#include "AboutDialog.h"
#include "QAlpacaSettings.h"
#include "QuitAppEvent.h"
#include "QTACDefines.h"
#include "QAlpacaDevice.h"

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
	_preferences.setAppName(kAppName, kAppVersion);
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

	AlpacaDevice tacDevice = QAlpacaDevice::findAlpacaDevice(QString::fromLatin1(portName));

	if (tacDevice && tacDevice->portName().empty() == false)
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

TACWindow* TACApplication::openByName(const QString& deviceName)
{
	return openByName(deviceName.toLatin1());
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
	QAlpacaSettings settings(kAppName);

	postEvent(this, new QuitAppEvent);
}

void TACApplication::on_preferencesChanged()
{
	emit preferencesChanged();
}

void TACApplication::shutDown()
{
	auto tacWindow = _tacWindows.begin();
	while (tacWindow != _tacWindows.end())
	{
		(*tacWindow)->shutDown();
		tacWindow++;
	}

	_tacWindows.clear();

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

	if (_tacWindows.count() == 0)
		tacAppInstance()->shutDown();

	return result;
}

bool TACApplication::openLastDevice()
{
	bool result{false};

	if (_starting)
	{
		_starting = false;

		if (_preferences.openLastStart())
		{
			QString lastDevice = _preferences.lastDevice();
			if (lastDevice.isEmpty() == false)
			{
				AlpacaDevice alpacaDevice = QAlpacaDevice::findAlpacaDevice(lastDevice);
				if (alpacaDevice == nullptr)
				{
					AppCore::writeToApplicationLogLine("LastDevice " + lastDevice.toStdString() + " not found");
					_preferences.saveLastDevice(QString());
					_preferences.setOpenLastStart(false);
				}
				else
				{
					AppCore::writeToApplicationLogLine("LastDevice: " + lastDevice.toStdString());

					TACWindow* tacWindow = openByName(lastDevice);
					if (tacWindow != Q_NULLPTR)
					{
						tacWindow->show();

						result = true;
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
