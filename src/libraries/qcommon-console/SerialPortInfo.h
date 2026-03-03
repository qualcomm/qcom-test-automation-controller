#ifndef SERIALPORTINFO_H
#define SERIALPORTINFO_H
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

#include "QCommonConsoleGlobal.h"

// Qt
#include <QList>
#include <QtSerialPort/QSerialPortInfo>

class SerialPortInfo;

typedef QList<SerialPortInfo> SerialPortInfos;

class QCOMMONCONSOLE_EXPORT SerialPortInfo :
	public QSerialPortInfo
{
public:
	SerialPortInfo() {};
	SerialPortInfo(const QString &name);
	SerialPortInfo(const SerialPortInfo& copyMe);
	SerialPortInfo(const QSerialPortInfo& copyMe);

	uint hash();

	static SerialPortInfos availablePorts();

    SerialPortInfo& operator = (const SerialPortInfo& copyMe) = default;
	bool operator == (const SerialPortInfo& compareMe);
	bool operator <= (const SerialPortInfo& compareMe);
	bool operator < (const SerialPortInfo& compareMe);

private:
	quint64						_hash{0};
};

bool equal(const SerialPortInfos& si1, const SerialPortInfos& si2);

inline bool operator ==
(
	const SerialPortInfo& si1,
	const SerialPortInfo& si2
)
{
	return si1.portName() == si2.portName();
}

inline bool operator <=
(
	const SerialPortInfo& si1,
	const SerialPortInfo& si2
)
{
	return si1.portName() <= si2.portName();
}

inline bool operator <
(
	const SerialPortInfo& si1,
	const SerialPortInfo& si2
)
{
	return si1.portName() < si2.portName();
}

quint64 qHash(const SerialPortInfo& key, uint seed);

#endif // SERIALPORTINFO_H
