// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "TACPIC32CXCoder.h"

#include "TACCommands.h"
#include "TACCommandHashes.h"

#include <string>
#include <vector>

TACPIC32CXCoder::TACPIC32CXCoder() :
	FrameCoder()
{
}

TACPIC32CXCoder::~TACPIC32CXCoder()
{
}

void TACPIC32CXCoder::reset()
{
	_recieveBuffer.clear();
	FrameCoder::reset();
}

void TACPIC32CXCoder::decode(const std::string& decodeMe)
{
	static const std::string kDelimiter{"\r\n"};

	_recieveBuffer += decodeMe;

	std::vector<std::string> frames;
	size_t start = 0;
	size_t pos = 0;
	while ((pos = _recieveBuffer.find(kDelimiter, start)) != std::string::npos)
	{
		if (pos > start)
			frames.push_back(_recieveBuffer.substr(start, pos - start));
		start = pos + kDelimiter.length();
	}
	if (start < _recieveBuffer.size())
		frames.push_back(_recieveBuffer.substr(start));

	if (_recieveBuffer.rfind(kPIC32CXCommandError, 0) == 0 || _recieveBuffer.find(kPIC32CXCommandNotRecognized) != std::string::npos)
	{
		_frameFunction(std::string(), _protocolInterface);

		_recieveBuffer.clear();
	}
	else if (_recieveBuffer.size() > kValidPIC32CXResponseSize)
	{
		if (frames.size() > 1)
			_frameFunction(frames[1], _protocolInterface);
		else if (!frames.empty())
			_frameFunction(frames[0], _protocolInterface);

		_frameFunction(std::string(), _protocolInterface);

		_recieveBuffer.clear();
	}
	else
	{
		_frameFunction(std::string(), _protocolInterface);

		_recieveBuffer.clear();
	}
}

std::string TACPIC32CXCoder::encode(const std::string& encodeMe, const Arguments &arguments)
{
	std::string result(encodeMe);

	switch (CommandStringToHash(encodeMe))
	{
	case kPIC32CXSetPinCommandHash:
		if (arguments.size() == 2)
		{
			std::string argumentStr = std::to_string(std::get<uint32_t>(arguments.at(1)));

			// Prepend a '0' to the pin to comply with pic32cx firmware if 'port 0' pins are triggered
			if (argumentStr.size() < 3)
				argumentStr = "0" + argumentStr;

			bool pinState = std::get<bool>(arguments.at(0));

			result = kPIC32CXSetPinCommand + " " + (pinState ? "1" : "0") + " (@" + argumentStr + ")";
		}
		break;

	default:
		result = encodeMe;
		break;
	}

	if (result.empty() || result.back() != '\n')
		result += "\n";

	return result;
}
