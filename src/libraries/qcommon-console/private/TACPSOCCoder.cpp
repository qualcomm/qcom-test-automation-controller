// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "TACPSOCCoder.h"

#include "TACCommands.h"
#include "TACCommandHashes.h"

TACPSOCCoder::TACPSOCCoder() :
	FrameCoder()
{
}

TACPSOCCoder::~TACPSOCCoder()
{
}

void TACPSOCCoder::TACPSOCCoder::reset()
{
	_recieveBuffer.clear();
	FrameCoder::reset();
}

void TACPSOCCoder::decode
(
	const QByteArray& decodeMe
)
{
	static const QString kDelimiter("\r\n");

	_recieveBuffer += decodeMe;

	QString frame = QString(_recieveBuffer);

	QStringList frames = frame.split(kDelimiter, Qt::SkipEmptyParts);

	if (frames.contains(kCommand) || frames.contains(kCommandNotRecognized))
	{
		int count(0);

		QListIterator<QString> iter(frames);
		while (iter.hasNext())
		{
			count++;
			QByteArray frameData = iter.next().toLatin1();
			_frameFunction(frameData, _protocolInterface);
		}

		while (count < 3)
		{
			_frameFunction(" ", _protocolInterface);
			count++;
		}

		_frameFunction(QByteArray(), _protocolInterface);

		_recieveBuffer.clear();
	}
}

QByteArray TACPSOCCoder::encode
(const QByteArray& encodeMe,
	const Arguments &arguments
)
{
	QByteArray result(encodeMe);

	switch (CommandStringToHash(encodeMe.toStdString()))
	{
	case kVersionCommandHash:
		result = "version\r";
		break;

	case kGetNameCommandHash:
		result = "getname\r";
		break;

	case kSetNameCommandHash:
		result = "setname " + QByteArray::fromStdString(std::get<std::string>(arguments.at(0))) + "\r";
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
		result = "i2c readRegisterBytes " + QByteArray::fromStdString(std::get<std::string>(arguments.at(0))) + " 1";
		break;

	case kI2CReadRegisterValueCommandHash:
		result = "i2c receive";
		break;

	case kI2CWriteRegisterCommandHash:
		if (arguments.size() == 3)
			result = "i2c write " + QByteArray::fromStdString(std::get<std::string>(arguments.at(0))) + " " + QByteArray::fromStdString(std::to_string(std::get<uint32_t>(arguments.at(1)))) + " " + QByteArray(std::get<bool>(arguments.at(2)) ? "1" : "0") + "\r";
		break;

	case kSetPinCommandHash:
		if (arguments.size() == 2)
			result = "pin " + QByteArray(std::get<bool>(arguments.at(0)) ? "1" : "0") + " " + QByteArray::fromStdString(std::to_string(std::get<uint32_t>(arguments.at(1)))) + "\r";
		break;

	default:
		result = encodeMe;
		break;
	}

	if (result.endsWith("\r") == false)
		result += "\r";

	return result;
}
