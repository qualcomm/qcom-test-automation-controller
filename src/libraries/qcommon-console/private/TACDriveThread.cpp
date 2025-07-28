// Confidential and Proprietary Qualcomm Technologies, Inc.

// NO PUBLIC DISCLOSURE PERMITTED:  Please report postings of this software on public servers or websites
// to: DocCtrlAgent@qualcomm.com.

// RESTRICTED USE AND DISCLOSURE:
// This software contains confidential and proprietary information and is not to be used, copied, reproduced, modified
// or distributed, in whole or in part, nor its contents revealed in any manner, without the express written permission
// of Qualcomm Technologies, Inc.

// Qualcomm is a trademark of QUALCOMM Incorporated, registered in the United States and other countries. All
// QUALCOMM Incorporated trademarks are used with permission.

// This software may be subject to U.S. and international export, re-export, or transfer laws.  Diversion contrary to U.S.
// and international law is strictly prohibited.

// Qualcomm Technologies, Inc.
// 5775 Morehouse Drive
// San Diego, CA 92121 U.S.A.
// Copyright 2013-2024 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
