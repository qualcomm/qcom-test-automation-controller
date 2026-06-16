// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "SerialPortInfo.h"

// QCommon
#include "StringUtilities.h"

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
		_hash = ::strHash(portName());
	}

	return _hash;
}

SerialPortInfos SerialPortInfo::availablePorts()
{
	SerialPortInfos result;

	QList<QSerialPortInfo> serialPortInfos = QSerialPortInfo::availablePorts();
	for (const auto& serialPortInfo: serialPortInfos)
	{
		result += SerialPortInfo(serialPortInfo);
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

	if (si1.count() == si2.count())
	{
		result = true;

		for (const auto& serialPortInfo: si1)
		{
			if (si2.contains(serialPortInfo) == false)
			{
				result = false;
				break;
			}
		}
	}

	return result;
}

quint64 qHash(const SerialPortInfo &key, uint seed)
{
	Q_UNUSED(seed)

	return qHash(key.portName());
}
