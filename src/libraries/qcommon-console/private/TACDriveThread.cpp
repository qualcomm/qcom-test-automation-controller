// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "TACDriveThread.h"

// QCommonConsole
#include "AlpacaDevice.h"
#include "AppCore.h"
#include "StringUtilities.h"
#include "TickCount.h"
#include "TACCommands.h"
#include "TACCommandHashes.h"
#include "TACLiteDriveThread.h"
#include "TACPSOCDriveThread.h"
#include "TACPIC32CXDriveThread.h"

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
			case ePSOC:
				result = new TACPSOCDriveThread(alpacaDevice->hash());
				break;

			case eFTDI:
				result = new TACLiteDriveThread(alpacaDevice->hash());
				break;

			case ePIC32CXAuto:
				result = new TACPIC32CXDriveThread(alpacaDevice->hash());

			default:
				break;
			}
		}
	}

	return result;
}

HashType TACDriveThread::hash()
{
	return _hash;
}

bool TACDriveThread::waitForCompletion()
{
	const int kMaxPolls{50};
	const int kPollIntervalMs{100};

	int count{0};

	while (_waitForCompletion)
	{
		QThread::msleep(kPollIntervalMs);	// Give our high priority time to process
		QCoreApplication::processEvents();  // Give the UI a timeslice to update

		if (count++ > kMaxPolls)
		{
			// The leading text must stay exactly "Wait for completion timed out"
			// because downstream consumers (TacService) match on that substring
			// to decide whether to reinitialize. Context is appended, not
			// substituted, so those matches keep working.
			AppCore::writeToApplicationLogLine(
				QString("Wait for completion timed out. Board stopped acknowledging after %1 ms"
						" (port=%2, driveTrain=%3, lastCommand=%4)")
					.arg(count * kPollIntervalMs)
					.arg(_portName.isEmpty() ? QByteArray("unknown") : _portName)
					.arg(_driveTrainName)
					.arg(_lastCommandDescription.isEmpty() ? QString("unknown") : _lastCommandDescription));

			_waitForCompletion = false;
			return false;
		}
	}

	// A command that completes but takes an unusually long time is an early
	// warning of the same fault. Without this the duration is only visible
	// buried in a frame dump, which is easy to miss.
	const int kSlowCommandPolls{10};
	if (count > kSlowCommandPolls)
	{
		AppCore::writeToApplicationLogLine(
			QString("Slow command completion: %1 ms (port=%2, lastCommand=%3)")
				.arg(count * kPollIntervalMs)
				.arg(_portName.isEmpty() ? QByteArray("unknown") : _portName)
				.arg(_lastCommandDescription.isEmpty() ? QString("unknown") : _lastCommandDescription));
	}

	return true;
}

// ----------------------------------------------------------------------------
// setLastCommandDescription
//
/// Records what the transport is currently doing so a timeout can name it.
///
/// Without this, "Wait for completion timed out" gives no indication of which
/// command stalled, and the command has to be inferred from surrounding log
/// lines after the fact.
// ----------------------------------------------------------------------------
void TACDriveThread::setLastCommandDescription(const QString& description)
{
	_lastCommandDescription = description;
}

// ----------------------------------------------------------------------------
// resetTransport
//
/// Posts a reset request to the drive thread and waits for it to complete.
///
/// Called from the thread issuing the command. The transport object is owned
/// and continuously used by run() on this QThread, so it must not be torn down
/// here - doing so would race with, and free memory under, the drive thread.
// ----------------------------------------------------------------------------
bool TACDriveThread::resetTransport()
{
	// A reset is pointless if the drive thread is not running to service it,
	// and waiting would block until the timeout for no reason.
	if (weAreRunning() == false)
	{
		AppCore::writeToApplicationLogLine("TACDriveThread::resetTransport: drive thread not running, cannot reset");
		return false;
	}

	QMutexLocker lock(&_resetMutex);

	_resetRequested = true;
	_resetDone = false;
	_resetResult = false;

	// Release the stale wait flag so run() is not stuck in a command that will
	// never complete; otherwise it may not reach the reset service point.
	_waitForCompletion = false;

	// Bounded wait: if the drive thread is itself wedged, report failure rather
	// than hanging the caller. The caller then surfaces the original timeout.
	const unsigned long kResetTimeoutMs{15000};

	while (_resetDone == false)
	{
		if (_resetCondition.wait(&_resetMutex, kResetTimeoutMs) == false)
		{
			AppCore::writeToApplicationLogLine("TACDriveThread::resetTransport: timed out waiting for drive thread to reset transport");

			// Withdraw the request so a later reset is not serviced by a stale
			// flag after this caller has already given up.
			_resetRequested = false;
			return false;
		}
	}

	return _resetResult;
}

// ----------------------------------------------------------------------------
// processPendingTransportReset
//
/// Services a reset posted by resetTransport(). Runs on the drive thread, so
/// performTransportReset() can safely touch the transport.
// ----------------------------------------------------------------------------
void TACDriveThread::processPendingTransportReset()
{
	bool shouldReset{false};

	{
		QMutexLocker lock(&_resetMutex);
		shouldReset = _resetRequested;
	}

	if (shouldReset == false)
		return;

	// Performed outside the lock: the reset reopens hardware and can take
	// seconds, and holding _resetMutex would serialise unrelated callers.
	bool result = performTransportReset();

	{
		QMutexLocker lock(&_resetMutex);

		// If the caller already timed out it cleared _resetRequested; the reset
		// still happened, so just drop the result rather than signalling.
		if (_resetRequested == true)
		{
			_resetRequested = false;
			_resetResult = result;
			_resetDone = true;
			_resetCondition.wakeAll();
		}
	}
}

void TACDriveThread::setWaitForCompletion()
{
	_waitForCompletion = true;
}

void TACDriveThread::clearWaitForCompletion()
{
	_waitForCompletion = false;
}

bool TACDriveThread::waitForCompletionStatus()
{
	return _waitForCompletion;
}

bool TACDriveThread::oldFirmware()
{
	return _oldFirmware;
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
		if (_platformID != MICRO_EPM_BOARD_ID_UNKNOWN)
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

uint TACDriveThread::majorVersion()
{
	return _firmwareMajor;
}

uint TACDriveThread::chipVersion()
{
	return _firmwareChip;
}

uint TACDriveThread::minorVersion()
{
	return _firmwareMinor;
}

uint TACDriveThread::revisionVersion()
{
	return _firmwareRevision;
}

QByteArray TACDriveThread::name() const
{
	return _name;
}

void TACDriveThread::setName(const QByteArray &newName)
{
	_name = newName;
}

QByteArray TACDriveThread::portName() const
{
	return _portName;
}

void TACDriveThread::setPortName(const QByteArray &portName)
{
	_portName = portName;
}

QString TACDriveThread::description() const
{
	return _description;
}

void TACDriveThread::setDescription(const QString &description)
{
	_description = description;
}

QString TACDriveThread::serialNumber() const
{
	return _serialNumber;
}

void TACDriveThread::setSerialNumber(QString serialNumber)
{
	_serialNumber = serialNumber;
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
