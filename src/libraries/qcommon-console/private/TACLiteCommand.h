#ifndef TACLITECOMMAND_H
#define TACLITECOMMAND_H
/*
	Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries. 
	 
	Redistribution and use in source and binary forms, with or without
	modification, are permitted (subject to the limitations in the
	disclaimer below) provided that the following conditions are met:
	 
		* Redistributions of source code must retain the above copyright
		  notice, this list of conditions and the following disclaimer.
	 
		* Redistributions in binary form must reproduce the above
		  copyright notice, this list of conditions and the following
		  disclaimer in the documentation and/or other materials provided
		  with the distribution.
	 
		* Neither the name of Qualcomm Technologies, Inc. nor the names of its
		  contributors may be used to endorse or promote products derived
		  from this software without specific prior written permission.
	 
	NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
	GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
	HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
	WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
	ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
	GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
	IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
	IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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
