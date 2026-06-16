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

	switch (CommandStringToHash(encodeMe))
	{
	case kVersionCommandHash:
		result = "version\r";
		break;

	case kGetNameCommandHash:
		result = "getname\r";
		break;

	case kSetNameCommandHash:
		result = "setname " + arguments.at(0).toByteArray() + "\r";
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
		result = "i2c readRegisterBytes " + arguments.at(0).toByteArray() + " 1";
		break;

	case kI2CReadRegisterValueCommandHash:
		result = "i2c receive";
		break;

	case kI2CWriteRegisterCommandHash:
		result = "i2c writeByte " + arguments.at(0).toByteArray();
		break;

	case kSetPinCommandHash:
		if (arguments.count() == 2)
			result = "pin " + variantToBoolString(arguments.at(0)) + " " + arguments.at(1).toByteArray()   + "\r";
		break;

	default:
		result = encodeMe;
		break;
	}

	if (result.endsWith("\r") == false)
		result += "\r";

	return result;
}
