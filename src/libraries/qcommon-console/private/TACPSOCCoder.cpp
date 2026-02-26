// Confidential and Proprietary – Qualcomm Technologies, Inc.

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
// Copyright ©2018-2023 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

// Author: msimpson

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
