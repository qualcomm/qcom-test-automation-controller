#ifndef TACLITECOMMAND_H
#define TACLITECOMMAND_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"

// QCommon
class ReceiveInterface;
#include "SendInterface.h"

// Qt
#include <QByteArray>

#include <cstdint>

class QCOMMONCONSOLE_EXPORT TACLiteCommand
{
public:
	TACLiteCommand(SendInterface* sender, ReceiveInterface* receiver);
	~TACLiteCommand();

	void version();
	void name();
	void uuid();

	virtual void setPinState(uint16_t pin, bool state);

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

	void i2CReadRegister(uint32_t addr, uint32_t reg);
	void i2CWriteRegister(uint32_t addr, uint32_t reg, uint32_t data);

	void send(const QByteArray& command, const Arguments& arguments, bool console, bool store = true);
	void addDelay(uint32_t delayInMilliSeconds);
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
