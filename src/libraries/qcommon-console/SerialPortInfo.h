// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SERIALPORTINFO_H
#define SERIALPORTINFO_H

#include "QCommonConsoleGlobal.h"

// Qt
#include <QtSerialPort/QSerialPortInfo>

#include <cstdint>
#include <string>
#include <vector>

class SerialPortInfo;

typedef std::vector<SerialPortInfo> SerialPortInfos;

class QCOMMONCONSOLE_EXPORT SerialPortInfo :
	public QSerialPortInfo
{
public:
	SerialPortInfo() {};
	SerialPortInfo(const std::string &name);
	SerialPortInfo(const QString &name);
	SerialPortInfo(const SerialPortInfo& copyMe);
	SerialPortInfo(const QSerialPortInfo& copyMe);

	uint32_t hash();

	static SerialPortInfos availablePorts();

    SerialPortInfo& operator = (const SerialPortInfo& copyMe) = default;
	bool operator == (const SerialPortInfo& compareMe);
	bool operator <= (const SerialPortInfo& compareMe);
	bool operator < (const SerialPortInfo& compareMe);

private:
	uint64_t					_hash{0};
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

uint64_t qHash(const SerialPortInfo& key, unsigned int seed);

#endif // SERIALPORTINFO_H
