// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef USBTOPOLOGY_H
#define USBTOPOLOGY_H

#include "QCommonConsoleGlobal.h"

#include <QByteArray>

class QCOMMONCONSOLE_EXPORT _USBTopology
{
public:
	_USBTopology() = delete;
	_USBTopology(const _USBTopology& copyMe) = delete;
	~_USBTopology() = delete;

	static QByteArray parentHubIdForFtdiDevice(const QByteArray& ftdiSerialNumber);
	static QByteArray parentHubIdForSerialPort(const QByteArray& portName);
};

#endif // USBTOPOLOGY_H
