#ifndef TACLITECOMMAND_H
#define TACLITECOMMAND_H
// Confidential and Proprietary – Qualcomm Technologies, Inc.

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
// Copyright ©2021-2023 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

// Author: msimpson

#include "QCommonConsoleGlobal.h"

// QCommon
class ReceiveInterface;
#include "SendInterface.h"

// Qt
#include <QByteArray>
#include <QVariant>

class QCOMMONCONSOLE_EXPORT TacLiteCommand
{
public:
	TacLiteCommand(SendInterface* sender, ReceiveInterface* receiver);
	~TacLiteCommand();

	void version();
	void name();
	void uuid();

	virtual void setPinState(quint16 pin, bool state);

	void battery(bool state);
	void usb0(bool state);
	void usb1(bool state);

	void externalPowerControl(bool state);

	void powerKey(bool onState);
	void volumeUp(bool onState);
	void volumeDown(bool onState);

	void setName(const QByteArray& newName);

	void getResetCount();
	void clearResetCount();

	void i2CReadRegister(quint32 addr, quint32 reg);
	void i2CWriteRegister(quint32 addr, quint32 reg, quint32 data);

	void send(const QByteArray& command, const Arguments& arguments, bool console, bool store = true);
	void addDelay(quint32 delayInMilliSeconds);
	void addLogComment(const QByteArray& comment);
	void addEndTransaction();

	// switches
	void disconnectUIM1Button(bool state);
	void disconnectUIM2Button(bool state);
	void forcePSHoldHigh(bool state);
	void disconnectSDCard(bool state);
	void primaryEDL(bool state);
	void secondaryEDL(bool state);
	void secondaryPMResinN(bool state);
	void eud(bool state);
	void platformID();
	void headsetDisconnect(bool state);

private:

	ReceiveInterface*			_receiver;
	SendInterface*				_sender;
	bool						_ready;
};

#endif // TACLITECOMMAND_H
