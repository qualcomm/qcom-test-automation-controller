// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "SerialPortInfo.h"

// QCommon
#include "StringUtilities.h"

#include <algorithm>
#include <string>

SerialPortInfo::SerialPortInfo
(
	const std::string& name
) :
	QSerialPortInfo(QString::fromStdString(name))
{
}

SerialPortInfo::SerialPortInfo
(
	const QString& name
) :
	QSerialPortInfo(name)
{
}

SerialPortInfo::SerialPortInfo
(
	const SerialPortInfo& copyMe
) :
	QSerialPortInfo(copyMe)
{
}

SerialPortInfo::SerialPortInfo
(
	const QSerialPortInfo& copyMe
) :
	QSerialPortInfo(copyMe)
{
}

uint SerialPortInfo::hash()
{
	if (_hash == 0)
	{
		_hash = ::strHash(portName().toStdString());
	}

	return _hash;
}

SerialPortInfos SerialPortInfo::availablePorts()
{
	SerialPortInfos result;

	QList<QSerialPortInfo> serialPortInfos = QSerialPortInfo::availablePorts();
	for (const auto& serialPortInfo: serialPortInfos)
	{
		result.push_back(SerialPortInfo(serialPortInfo));
	}

	return result;
}

bool SerialPortInfo::operator ==
(
	const SerialPortInfo& compareMe
)
{
	return portName() == compareMe.portName();
}

bool SerialPortInfo::operator <=
(
	const SerialPortInfo& compareMe
)
{
	return portName() <= compareMe.portName();
}

bool SerialPortInfo::operator <
(
	const SerialPortInfo& compareMe
)
{
	return portName() < compareMe.portName();
}

bool equal
(
	const SerialPortInfos& si1,
	const SerialPortInfos& si2
)
{
	bool result(false);

	if (si1.size() == si2.size())
	{
		result = true;

		for (const auto& serialPortInfo: si1)
		{
			if (std::find(si2.begin(), si2.end(), serialPortInfo) == si2.end())
			{
				result = false;
				break;
			}
		}
	}

	return result;
}

uint64_t qHash(const SerialPortInfo &key, unsigned int seed)
{
	Q_UNUSED(seed)

	return qHash(key.portName());
}
