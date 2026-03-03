#ifndef TACPIC32CXCOMMAND_H
#define TACPIC32CXCOMMAND_H

// Confidential and Proprietary Ã¢â‚¬â€œ Qualcomm Technologies, Inc.

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
// Copyright 2018-2024 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

// Author: Biswajit Roy (biswroy@qti.qualcomm.com)

#include "QCommonConsoleGlobal.h"

// QCommonConsole
class ReceiveInterface;
#include "SendInterface.h"

// Qt
#include <QByteArray>
#include <QVariant>


class QCOMMONCONSOLE_EXPORT TACPIC32CXCommand
{
public:
	TACPIC32CXCommand(SendInterface* sender, ReceiveInterface* receiver);
	~TACPIC32CXCommand();

	void version();
	void name();
	void uuid();

	virtual void setPinState(quint16 pin, bool state);

	void setName(const QByteArray& newName);

	void send(const QByteArray& command, const Arguments& arguments, bool console, bool store = true);
	void addDelay(quint32 delayInMilliSeconds);
	void addLogComment(const QByteArray& comment);
	void addEndTransaction();

	void platformID();
	void clearBuffer();
private:
	ReceiveInterface*			_receiver;
	SendInterface*				_sender;
	bool						_ready;
};


#endif // TACPIC32CXCOMMAND_H
