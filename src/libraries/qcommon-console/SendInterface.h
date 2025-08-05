#ifndef SENDINTERFACE_H
#define SENDINTERFACE_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// QCommon
#include "QCommonConsoleGlobal.h"
#include "ReceiveInterface.h"

// Qt
#include <QByteArray>
#include <QVariant>

typedef QList<QVariant> Arguments;

class QCOMMONCONSOLE_EXPORT SendInterface
{
public:
	SendInterface() {}
	virtual ~SendInterface() {}

	virtual bool ready() = 0;
	virtual quint32 send(const QByteArray& sendMe, const Arguments& argument, bool command, ReceiveInterface* recieveInterface, bool store = true) = 0;
	virtual void addDelay(quint32 delayInMilliSeconds, ReceiveInterface* recieveInterface) = 0;
	virtual void addLogComment(const QByteArray& comment) = 0;
	virtual void addEndTransaction(ReceiveInterface* recieveInterface) = 0;
};

#endif // SENDINTERFACE_H

