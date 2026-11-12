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
	int count{0};

	while (_waitForCompletion)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));				// Give our high priority time to process
		QCoreApplication::processEvents();  // Give the UI a timeslice to update

		if (count++ > 50)
		{
			AppCore::writeToApplicationLogLine("Wait for completion timed out.");

			_waitForCompletion = false;
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

static std::string trimString(const std::string& s)
{
	size_t first = s.find_first_not_of(" \t\r\n");
	if (first == std::string::npos) return "";
	size_t last = s.find_last_not_of(" \t\r\n");
	return s.substr(first, (last - first + 1));
}

static std::string toLowerString(const std::string& s)
{
	std::string res = s;
	for (char& c : res)
		c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
	return res;
}

static bool startsWithCaseInsensitive(const std::string& str, const std::string& prefix)
{
	if (str.size() < prefix.size()) return false;
	for (size_t i = 0; i < prefix.size(); ++i)
	{
		if (std::tolower(static_cast<unsigned char>(str[i])) != std::tolower(static_cast<unsigned char>(prefix[i])))
			return false;
	}
	return true;
}

static bool endsWithCaseInsensitive(const std::string& str, const std::string& suffix)
{
	if (str.size() < suffix.size()) return false;
	size_t offset = str.size() - suffix.size();
	for (size_t i = 0; i < suffix.size(); ++i)
	{
		if (std::tolower(static_cast<unsigned char>(str[offset + i])) != std::tolower(static_cast<unsigned char>(suffix[i])))
			return false;
	}
	return true;
}

std::string TACDriveThread::decodeCommand
(
	const std::string& command,
	Arguments& args
)
{
	std::string result = trimString(toLowerString(command));

	if (endsWithCaseInsensitive(result, " on"))
	{
		args.push_back(true);
		result = result.substr(0, result.size() - 3);
	}
	else if (endsWithCaseInsensitive(result, " off"))
	{
		args.push_back(false);
		result = result.substr(0, result.size() - 4);
	}
	else if (endsWithCaseInsensitive(result, " 1"))
	{
		args.push_back(true);
		result = result.substr(0, result.size() - 2);
	}
	else if (endsWithCaseInsensitive(result, " 0"))
	{
		args.push_back(false);
		result = result.substr(0, result.size() - 2);
	}

	if (startsWithCaseInsensitive(result, kSetNameCommand) || startsWithCaseInsensitive(result, "setname"))
	{
		size_t prefixLen = startsWithCaseInsensitive(result, kSetNameCommand) ? kSetNameCommand.size() : std::string("setname").size();
		result = trimString(result.substr(prefixLen));

		args.push_back(result);
		result = kSetNameCommand;
	}
	else if (startsWithCaseInsensitive(result, kSetButtonAssertTime) || startsWithCaseInsensitive(result, kSetButtonAssertTimeAlias))
	{
		size_t prefixLen = startsWithCaseInsensitive(result, kSetButtonAssertTime) ? kSetButtonAssertTime.size() : kSetButtonAssertTimeAlias.size();
		result = trimString(result.substr(prefixLen));

		try {
			args.push_back(static_cast<uint32_t>(std::stoul(result)));
		} catch (...) {}
		result = kSetButtonAssertTime;
	}
	else if (startsWithCaseInsensitive(result, kSetPowerKeyDelay) || startsWithCaseInsensitive(result, kSetPowerKeyDelayAlias))
	{
		size_t prefixLen = startsWithCaseInsensitive(result, kSetPowerKeyDelay) ? kSetPowerKeyDelay.size() : kSetPowerKeyDelayAlias.size();
		result = trimString(result.substr(prefixLen));

		try {
			args.push_back(static_cast<uint32_t>(std::stoul(result)));
		} catch (...) {}
		result = kSetPowerKeyDelay;
	}
	else if (startsWithCaseInsensitive(result, kSetPinCommand))
	{
		result = trimString(result.substr(kSetPinCommand.size()));

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

std::string TACDriveThread::firmwareVersion()
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

std::string TACDriveThread::name() const
{
	return _name;
}

void TACDriveThread::setName(const std::string &newName)
{
	_name = newName;
}

std::string TACDriveThread::portName() const
{
	return _portName;
}

void TACDriveThread::setPortName(const std::string &portName)
{
	_portName = portName;
}

std::string TACDriveThread::description() const
{
	return _description;
}

void TACDriveThread::setDescription(const std::string &description)
{
	_description = description;
}

std::string TACDriveThread::serialNumber() const
{
	return _serialNumber;
}

void TACDriveThread::setSerialNumber(const std::string &serialNumber)
{
	_serialNumber = serialNumber;
}

std::string TACDriveThread::uuid()
{
	return _uuid;
}

std::string TACDriveThread::macAddress()
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
	static uint64_t lastTimeStamp{0};

	if (lastTimeStamp == 0)
	{
		lastTimeStamp = tickCount();
	}

	uint64_t current = tickCount();
	timeStampMe += "Time: " + std::to_string(current) + " elapsed: " + std::to_string(current - lastTimeStamp) + " (ms)";
	lastTimeStamp = current;
}

void TACDriveThread::setThreadDelay(const uint delay)
{
	_delay = delay;
}
