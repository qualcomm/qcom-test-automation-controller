#include "TACPIC32CXCoder.h"

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
// Copyright ©2024 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

// Author: Biswajit Roy (biswroy@qti.qualcomm.com)

#include "TACPIC32CXCoder.h"

#include "TACCommands.h"
#include "TACCommandHashes.h"

TACPIC32CXCoder::TACPIC32CXCoder() :
	FrameCoder()
{
}

TACPIC32CXCoder::~TACPIC32CXCoder()
{
}

void TACPIC32CXCoder::TACPIC32CXCoder::reset()
{
	_recieveBuffer.clear();
	FrameCoder::reset();
}

void TACPIC32CXCoder::decode(const QByteArray& decodeMe)
{
	static const QString kDelimiter("\r\n");

	_recieveBuffer += decodeMe;

	QString frame = QString(_recieveBuffer);

	QStringList frames = frame.split(kDelimiter, Qt::SkipEmptyParts);

	if (_recieveBuffer.startsWith(kPIC32CXCommandError) || _recieveBuffer.contains(kPIC32CXCommandNotRecognized))
	{
		_frameFunction(QByteArray(), _protocolInterface);

		_recieveBuffer.clear();
	}

	else if (_recieveBuffer.size() > kValidPIC32CXResponseSize)
	{
		QListIterator<QString> iter(frames);

		_frameFunction(frames[1].toLatin1(), _protocolInterface);
		_frameFunction(QByteArray(), _protocolInterface);

		_recieveBuffer.clear();
	}
	else
	{
		_frameFunction(QByteArray(), _protocolInterface);

		_recieveBuffer.clear();
	}
}

QByteArray TACPIC32CXCoder::encode(const QByteArray& encodeMe, const Arguments &arguments)
{
	QByteArray result(encodeMe);

	switch (CommandStringToHash(encodeMe))
	{
	case kPIC32CXSetPinCommandHash:
		if (arguments.count() == 2)
		{
			QByteArray argumentStr = arguments.at(1).toByteArray();

			// Prepend a '0' to the pin to comply with pic32cx firmware if 'port 0' pins are triggered
			if (argumentStr.size() < 3)
				argumentStr = "0" + argumentStr;

			result = kPIC32CXSetPinCommand + " " + variantToBoolString(arguments.at(0)) + " (@" + argumentStr + ")";
		}
		break;

	default:
		result = encodeMe;
		break;
	}

	if (result.endsWith("\n") == false)
		result += "\n";

	return result;
}
