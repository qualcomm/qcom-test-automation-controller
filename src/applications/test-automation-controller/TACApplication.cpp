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

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "TACApplication.h"

// TAC
#include "TACWindow.h"

// libTAC
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
			QByteArray lastDevice = _preferences.lastDevice().toLatin1();
			if (lastDevice.isEmpty() == false)
			{
				AlpacaDevice alpacaDevice = _AlpacaDevice::findAlpacaDevice(lastDevice);
				if (alpacaDevice.isNull())
				{
					AppCore::writeToApplicationLogLine(QString("LastDevice %1 not found").arg(QString(lastDevice)));
					_preferences.saveLastDevice("");
					_preferences.setOpenLastStart(false);
				}
				else
				{
					AppCore::writeToApplicationLogLine(QString("LastDevice: %1").arg(QString(lastDevice)));

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
