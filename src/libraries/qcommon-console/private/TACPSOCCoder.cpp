// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "TACPSOCCoder.h"

#include "TACCommands.h"
#include "TACCommandHashes.h"

#include <string>
#include <vector>

TACPSOCCoder::TACPSOCCoder() :
	FrameCoder()
{
}

TACPSOCCoder::~TACPSOCCoder()
{
}

void TACPSOCCoder::reset()
{
	_recieveBuffer.clear();
	FrameCoder::reset();
}

void TACPSOCCoder::decode
(
	const std::string& decodeMe
)
{
	static const std::string kDelimiter{"\r\n"};

	_recieveBuffer += decodeMe;

	std::vector<std::string> frames;
	size_t start = 0, pos = 0;
	while ((pos = _recieveBuffer.find(kDelimiter, start)) != std::string::npos)
	{
		if (pos > start)
			frames.push_back(_recieveBuffer.substr(start, pos - start));
		start = pos + kDelimiter.length();
	}
	if (start < _recieveBuffer.size())
		frames.push_back(_recieveBuffer.substr(start));

	bool hasCmd = false;
	for (const auto& f : frames)
	{
		if (f == kCommand || f == kCommandNotRecognized)
		{
			hasCmd = true;
			break;
		}
	}

	if (hasCmd)
	{
		int count(0);

		for (const auto& frameData : frames)
		{
			count++;
			_frameFunction(frameData, _protocolInterface);
		}

		while (count < 3)
		{
			_frameFunction(" ", _protocolInterface);
			count++;
		}

		_frameFunction(std::string(), _protocolInterface);

		_recieveBuffer.clear();
	}
}

std::string TACPSOCCoder::encode
(
	const std::string& encodeMe,
	const Arguments &arguments
)
{
	std::string result(encodeMe);

	switch (CommandStringToHash(encodeMe))
	{
	case kVersionCommandHash:
		result = "version\r";
		break;

	case kGetNameCommandHash:
		result = "getname\r";
		break;

	case kSetNameCommandHash:
		result = "setname " + std::get<std::string>(arguments.at(0)) + "\r";
		break;

	case kGetUUIDCommandHash:
		result = "sys getFSUUID\r";
		break;

	case kGetPlatformIDCommandHash:
		result = "getboardid\r";
		break;

	case kGetResetCountCommandHash:
		result = "getresetcount\r";
		break;

	case kClearResetCountCommandHash:
		result = "clearresetcount\r";
		break;

	case kI2CReadRegisterCommandHash:
		result = "i2c readRegisterBytes " + std::get<std::string>(arguments.at(0)) + " 1";
		break;

	case kI2CReadRegisterValueCommandHash:
		result = "i2c receive";
		break;

	case kI2CWriteRegisterCommandHash:
		if (arguments.size() == 3)
			result = "i2c write " + std::get<std::string>(arguments.at(0)) + " " + std::to_string(std::get<uint32_t>(arguments.at(1))) + " " + (std::get<bool>(arguments.at(2)) ? "1" : "0") + "\r";
		break;

	case kSetPinCommandHash:
		if (arguments.size() == 2)
			result = "pin " + std::string(std::get<bool>(arguments.at(0)) ? "1" : "0") + " " + std::to_string(std::get<uint32_t>(arguments.at(1))) + "\r";
		break;

	default:
		result = encodeMe;
		break;
	}

	if (result.empty() || result.back() != '\r')
		result += "\r";

	return result;
}
