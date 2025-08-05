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
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "TACDriveThread.h"


// qcommon-console
#include "AlpacaDevice.h"
#include "AppCore.h"
#include "StringUtilities.h"
#include "TickCount.h"
#include "TACCommands.h"
#include "TacCommandHashes.h"
#include "TACLiteDriveThread.h"

// Qt
#include <QDir>
#include <QDateTime>
#include <QTextStream>

TACDriveThread::TACDriveThread
(
	HashType hash
) :
	_hash(hash)
{
}

TACDriveThread::~TACDriveThread()
{
}

TACDriveThread* TACDriveThread::openPort
(
	const QByteArray& portName
)
{
	TACDriveThread* result{Q_NULLPTR};

	if (_AlpacaDevice::updateAlpacaDevices() > 0)
	{
		AlpacaDevice alpacaDevice = _AlpacaDevice::findAlpacaDevice(portName);

		if (alpacaDevice->active())
		{
			switch (alpacaDevice->debugBoardType())
			{
			case eFTDI:
				result = new TacLiteDriveThread(alpacaDevice->hash());
				break;

			default:
				break;
			}
		}
	}

	return result;
}

QByteArray TACDriveThread::decodeCommand
(
	const QByteArray& command,
	Arguments& args
)
{
	QString result{command.toLower()};

	result = result.trimmed();

	if (result.endsWith(" on"))
	{
		args.push_back(true);
		result.remove(" on");
	}
	else if (result.endsWith(" off"))
	{
		args.push_back(false);
		result.remove(" off");
	}
	else if (result.endsWith(" 1"))
	{
		args.push_back(true);
		result.remove(" 1");
	}
	else if (result.endsWith(" 0"))
	{
		args.push_back(false);
		result.remove(" 0");
	}

	if (result.startsWith(kSetNameCommand) || result.startsWith("setname"))
	{
		result.remove(kSetNameCommand.toLower());
		result = result.remove("setname").trimmed();

		args.push_back(result.toLatin1());
		result = kSetNameCommand;
	}
	else if (result.startsWith(kSetButtonAssertTime) || result.startsWith(kSetButtonAssertTimeAlias))
	{
		result.remove(kSetNameCommand.toLower());
		result = result.remove(kSetButtonAssertTimeAlias).trimmed();

		args.push_back(result.toUInt());
		result = kSetButtonAssertTime;
	}
	else if (result.startsWith(kSetPowerKeyDelay) || result.startsWith(kSetPowerKeyDelayAlias))
	{
		result.remove(kSetNameCommand.toLower());
		result = result.remove(kSetPowerKeyDelayAlias).trimmed();

		args.push_back(result.toUInt());
		result = kSetPowerKeyDelay;
	}
	else if (result.startsWith(kSetPinCommand, Qt::CaseInsensitive))
	{
		result.remove(kSetPinCommand.toLower());

		QStringList argList = result.split(" ", Qt::SkipEmptyParts);
		args.push_back(result.toUInt()); // state and then pin

		result = kSetPinCommand;
	}

	quint32 hash = CommandStringToHash(result.toLatin1());
	if (hash != 0)
	{
		oldCommandEntry commandEntry = CommandHashToCommandEntry(hash);
		result = commandEntry._longCommand;
	}
	else
		result.clear();

	return result.toLatin1();
}

bool TACDriveThread::checkLocalStore
(
	FramePackage& framePackage
)
{
	bool result{false};
	QString response;

	switch(framePackage->_requestHash)
	{
	case kVersionCommandHash:
		if (_versionString.isEmpty() == false)
		{
			framePackage->_responses.push_back(_versionString);
			result = true;
		}
		break;

	case kGetNameCommandHash:
		if (_name.isEmpty() == false)
		{
			framePackage->_responses.push_back(_name);
			result = true;
		}
		break;

	case kGetUUIDCommandHash:
		if (_uuid.isEmpty() == false)
		{
			framePackage->_responses.push_back(_uuid.toLatin1());
			result = true;
		}
		break;

	case kGetPlatformIDCommandHash:
		{
			QString response = QString("%1(%2)").arg(PlatformContainer::toString(_platformID)).arg(static_cast<int>(_platformID));
			framePackage->_responses.push_back(response.toLatin1().data());
			result = true;
		}
		break;

	case kPIC32CXVersionCommandHash:
		if (_versionString.isEmpty() == false)
		{
			framePackage->_responses.push_back(_versionString);
			result = true;
		}
		break;
	}

	return result;
}

DebugBoardType TACDriveThread::debugBoardType()
{
	return _hardwareType;
}

QString TACDriveThread::debugBoardTypeString()
{
	return debugBoardTypeToString(_hardwareType);
}

PlatformID TACDriveThread::platformID()
{
	return _platformID;
}

QString TACDriveThread::hardwareVersionString()
{
	return PlatformContainer::toString(_platformID);
}

QString TACDriveThread::firmwareVersion()
{
	return _firmwareString;
}

QString TACDriveThread::uuid()
{
	return _uuid;
}

QByteArray TACDriveThread::macAddress()
{
	return _macAddress;
}

void TACDriveThread::setupLogging(bool loggingState)
{
	AppCore::getAppCore()->setRunLogging(loggingState);
}

void TACDriveThread::shutdownLogging()
{
	AppCore::getAppCore()->setRunLogging(false);
}

bool TACDriveThread::resetLogging()
{
	bool result{false};

	AppCore* appCore = AppCore::getAppCore();
	if (appCore != Q_NULLPTR)
	{
		result = appCore->runLoggingActive();
		if (result)
		{
			appCore->setRunLogging(false);
			appCore->setRunLogging(true);
		}
	}
	return result;
}

void TACDriveThread::log
(
	FramePackage& framePackage
)
{
	QString logEntry;

	AppCore::writeToApplicationLogLine("");
	AppCore::writeToApplicationLogLine("Frame Package Start");

	AppCore::writeToApplicationLogLine(framePackage->_valid ? "Frame Valid" : "Frame Invalid");

	if (framePackage->_delayInMilliSeconds > 0)
	{
		logEntry = QString(" Delay %1 in msecs").arg(framePackage->_delayInMilliSeconds);
		timeStampLogMessage(logEntry);
		AppCore::writeToApplicationLogLine(logEntry);
	}
	else if (framePackage->_comment.isEmpty() == false)
	{
		logEntry = QString("%1").arg(framePackage->_comment.data());
		AppCore::writeToApplicationLogLine(logEntry);
	}
	else
	{
		if (framePackage->_console == true)
			logEntry = QString("Request from console: %1").arg(framePackage->_request.data());
		else
			logEntry = QString("Request: %1").arg(framePackage->_request.data());

		AppCore::writeToApplicationLogLine(logEntry);

		if (framePackage->_synonym.isEmpty() == false)
		{
			logEntry = QString("Synonym: %1").arg(framePackage->_synonym.data());
			AppCore::writeToApplicationLogLine(logEntry);
		}

		AppCore::writeToApplicationLogLine("Responses");

		auto response = framePackage->_responses.begin();
		while (response != framePackage->_responses.end())
		{
			logEntry = QString("   %1").arg(response->data());
			AppCore::writeToApplicationLogLine(logEntry);
			response++;
		}
	}

	AppCore::writeToApplicationLogLine("Frame Package End");
	AppCore::writeToApplicationLogLine("");
}

void TACDriveThread::timeStampLogMessage
(
	QString& timeStampMe
)
{
	static quint64 lastTimeStamp{0};

	if (lastTimeStamp == 0)
	{
		lastTimeStamp = tickCount();
	}

	quint64 current = tickCount();
	timeStampMe += QString("Time: %1 elapsed: %2 (ms)").arg(current).arg(current - lastTimeStamp);
	lastTimeStamp = current;
}

void TACDriveThread::setThreadDelay(const uint delay)
{
	_delay = delay;
}
