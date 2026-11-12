// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

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

	switch (CommandStringToHash(encodeMe.toStdString()))
	{
	case kPIC32CXSetPinCommandHash:
		if (arguments.size() == 2)
		{
			QByteArray argumentStr = QByteArray::fromStdString(std::to_string(std::get<uint32_t>(arguments.at(1))));

			// Prepend a '0' to the pin to comply with pic32cx firmware if 'port 0' pins are triggered
			if (argumentStr.size() < 3)
				argumentStr = "0" + argumentStr;

			bool pinState = std::get<bool>(arguments.at(0));

			result = kPIC32CXSetPinCommand + " " + QByteArray(pinState ? "1" : "0") + " (@" + argumentStr + ")";
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
