#ifndef SENDINTERFACE_H
#define SENDINTERFACE_H

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
// Copyright 2013-2021 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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

