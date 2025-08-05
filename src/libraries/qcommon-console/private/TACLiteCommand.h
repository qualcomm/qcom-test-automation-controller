#ifndef TACLITECOMMAND_H
#define TACLITECOMMAND_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

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
