// Confidential and Proprietary Qualcomm Technologies, Inc.

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
// Copyright 2019 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

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
