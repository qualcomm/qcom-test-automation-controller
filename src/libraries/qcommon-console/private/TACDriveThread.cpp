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

#include <cctype>
#include <chrono>
#include <cstdint>
#include <string>
#include <thread>

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
	const std::string& portName
)
{
	TACDriveThread* result{nullptr};

	if (_AlpacaDevice::updateAlpacaDevices() > 0)
	{
		AlpacaDevice alpacaDevice = _AlpacaDevice::findAlpacaDevice(portName);

		if (alpacaDevice && alpacaDevice->active())
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

void TACDriveThread::waitForCompletion()
{
	std::unique_lock<std::mutex> lock(_completionMutex);
	bool completed = _completionCv.wait_for(lock, std::chrono::milliseconds(5000), [this]() {
		return !_waitForCompletion || stopRequested();
	});

	if (!completed || _waitForCompletion)
	{
		AppCore::writeToApplicationLogLine("Wait for completion timed out.");
		_waitForCompletion = false;
	}
}

void TACDriveThread::setWaitForCompletion()
{
	std::lock_guard<std::mutex> lock(_completionMutex);
	_waitForCompletion = true;
}

void TACDriveThread::clearWaitForCompletion()
{
	{
		std::lock_guard<std::mutex> lock(_completionMutex);
		_waitForCompletion = false;
	}
	_completionCv.notify_all();
}

void TACDriveThread::onStopping()
{
	clearWaitForCompletion();
}

bool TACDriveThread::waitForCompletionStatus()
{
	return _waitForCompletion;
}

bool TACDriveThread::oldFirmware() const
{
	return _oldFirmware;
}

std::string TACDriveThread::decodeCommand
(
	const std::string& command,
	Arguments& args
)
{
	std::string result = trim(toLower(command));

	if (endsWith(result, " on", false))
	{
		args.push_back(true);
		result = result.substr(0, result.size() - 3);
	}
	else if (endsWith(result, " off", false))
	{
		args.push_back(false);
		result = result.substr(0, result.size() - 4);
	}
	else if (endsWith(result, " 1", false))
	{
		args.push_back(true);
		result = result.substr(0, result.size() - 2);
	}
	else if (endsWith(result, " 0", false))
	{
		args.push_back(false);
		result = result.substr(0, result.size() - 2);
	}

	if (startsWith(result, kSetNameCommand, false) || startsWith(result, "setname", false))
	{
		size_t prefixLen = startsWith(result, kSetNameCommand, false) ? kSetNameCommand.size() : std::string("setname").size();
		result = trim(result.substr(prefixLen));

		args.push_back(result);
		result = kSetNameCommand;
	}
	else if (startsWith(result, kSetButtonAssertTime, false) || startsWith(result, kSetButtonAssertTimeAlias, false))
	{
		size_t prefixLen = startsWith(result, kSetButtonAssertTime, false) ? kSetButtonAssertTime.size() : kSetButtonAssertTimeAlias.size();
		result = trim(result.substr(prefixLen));

		try {
			args.push_back(static_cast<uint32_t>(std::stoul(result)));
		} catch (...) {}
		result = kSetButtonAssertTime;
	}
	else if (startsWith(result, kSetPowerKeyDelay, false) || startsWith(result, kSetPowerKeyDelayAlias, false))
	{
		size_t prefixLen = startsWith(result, kSetPowerKeyDelay, false) ? kSetPowerKeyDelay.size() : kSetPowerKeyDelayAlias.size();
		result = trim(result.substr(prefixLen));

		try {
			args.push_back(static_cast<uint32_t>(std::stoul(result)));
		} catch (...) {}
		result = kSetPowerKeyDelay;
	}
	else if (startsWith(result, kSetPinCommand, false))
	{
		result = trim(result.substr(kSetPinCommand.size()));

		try {
			args.push_back(static_cast<uint32_t>(std::stoul(result)));
		} catch (...) {}
		result = kSetPinCommand;
	}

	uint32_t hash = CommandStringToHash(result);
	if (hash != 0)
	{
		oldCommandEntry commandEntry = CommandHashToCommandEntry(hash);
		result = commandEntry._longCommand;
	}
	else
		result.clear();

	return result;
}

bool TACDriveThread::checkLocalStore
(
	FramePackage& framePackage
)
{
	bool result{false};

	switch(framePackage->_requestHash)
	{
	case kVersionCommandHash:
		if (!_versionString.empty())
		{
			framePackage->_responses.push_back(_versionString);
			result = true;
		}
		break;

	case kGetNameCommandHash:
		if (!_name.empty())
		{
			framePackage->_responses.push_back(_name);
			result = true;
		}
		break;

	case kGetUUIDCommandHash:
		if (!_uuid.empty())
		{
			framePackage->_responses.push_back(_uuid);
			result = true;
		}
		break;

	case kGetPlatformIDCommandHash:
		if (_platformID != MICRO_EPM_BOARD_ID_UNKNOWN)
		{
			std::string response = PlatformContainer::toString(_platformID) + "(" + std::to_string(static_cast<int>(_platformID)) + ")";
			framePackage->_responses.push_back(response);
			result = true;
		}
		break;

	case kPIC32CXVersionCommandHash:
		if (!_versionString.empty())
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

std::string TACDriveThread::debugBoardTypeString()
{
	return debugBoardTypeToString(_hardwareType);
}

PlatformID TACDriveThread::platformID()
{
	return _platformID;
}

std::string TACDriveThread::hardwareVersionString()
{
	return PlatformContainer::toString(_platformID);
}

const std::string& TACDriveThread::firmwareVersion() const
{
	return _firmwareString;
}

uint32_t TACDriveThread::majorVersion() const
{
	return _firmwareMajor;
}

uint32_t TACDriveThread::chipVersion() const
{
	return _firmwareChip;
}

uint32_t TACDriveThread::minorVersion() const
{
	return _firmwareMinor;
}

uint32_t TACDriveThread::revisionVersion() const
{
	return _firmwareRevision;
}

const std::string& TACDriveThread::name() const
{
	return _name;
}

void TACDriveThread::setName(const std::string &newName)
{
	_name = newName;
}

const std::string& TACDriveThread::portName() const
{
	return _portName;
}

void TACDriveThread::setPortName(const std::string &portName)
{
	_portName = portName;
}

const std::string& TACDriveThread::description() const
{
	return _description;
}

void TACDriveThread::setDescription(const std::string &description)
{
	_description = description;
}

const std::string& TACDriveThread::serialNumber() const
{
	return _serialNumber;
}

void TACDriveThread::setSerialNumber(const std::string &serialNumber)
{
	_serialNumber = serialNumber;
}

const std::string& TACDriveThread::uuid() const
{
	return _uuid;
}

const std::string& TACDriveThread::macAddress() const
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
	if (appCore != nullptr)
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
	std::string logEntry;

	AppCore::writeToApplicationLogLine("");
	AppCore::writeToApplicationLogLine("Frame Package Start");

	AppCore::writeToApplicationLogLine(framePackage->_valid ? "Frame Valid" : "Frame Invalid");

	if (framePackage->_delayInMilliSeconds > 0)
	{
		logEntry = " Delay " + std::to_string(framePackage->_delayInMilliSeconds) + " in msecs";
		timeStampLogMessage(logEntry);
		AppCore::writeToApplicationLogLine(logEntry);
	}
	else if (!framePackage->_comment.empty())
	{
		AppCore::writeToApplicationLogLine(framePackage->_comment);
	}
	else
	{
		if (framePackage->_console)
			logEntry = "Request from console: " + framePackage->_request;
		else
			logEntry = "Request: " + framePackage->_request;

		AppCore::writeToApplicationLogLine(logEntry);

		if (!framePackage->_synonym.empty())
		{
			logEntry = "Synonym: " + framePackage->_synonym;
			AppCore::writeToApplicationLogLine(logEntry);
		}

		AppCore::writeToApplicationLogLine("Responses");

		for (const auto& response : framePackage->_responses)
		{
			logEntry = "   " + response;
			AppCore::writeToApplicationLogLine(logEntry);
		}
	}

	AppCore::writeToApplicationLogLine("Frame Package End");
	AppCore::writeToApplicationLogLine("");
}

void TACDriveThread::timeStampLogMessage
(
	std::string& timeStampMe
)
{
	if (_lastTimeStamp == 0)
	{
		_lastTimeStamp = tickCount();
	}

	uint64_t current = tickCount();
	timeStampMe += "Time: " + std::to_string(current) + " elapsed: " + std::to_string(current - _lastTimeStamp) + " (ms)";
	_lastTimeStamp = current;
}

void TACDriveThread::setThreadDelay(const uint32_t delay)
{
	_delay = delay;
}
