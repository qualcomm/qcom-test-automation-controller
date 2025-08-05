#ifndef TACLITEPROTOCOL_H
#define TACLITEPROTOCOL_H
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

// AlpacaServer
class TACDriveThread;

// QCommon
#include "ProtocolInterface.h"
#include "ReceiveInterface.h"
#include "SendInterface.h"

class QCOMMONCONSOLE_EXPORT TACLiteProtocol :
	public QObject,
	public ProtocolInterface,
	public ReceiveInterface
{
Q_OBJECT

Q_INTERFACES(ProtocolInterface)

public:
	TACLiteProtocol();
	virtual ~TACLiteProtocol();

	void setTacDriveTrain(TACDriveThread* tacDriveTrain);

	quint32 sendCommand(const QByteArray& command, const Arguments& arguments, bool console = false,
		ReceiveInterface* receiveInterface = Q_NULLPTR, bool shouldStore = true);
	void endTransaction(ReceiveInterface* receiveInterface = Q_NULLPTR);
	void sendHelpCommand();

	// Receive Interface
	virtual void receive(FramePackage& framePackage);

	// timing
	virtual void idle();

protected:
	virtual void frameComplete(const QByteArray& completedFrame);
	virtual void badFrame(const QByteArray& completedFrame);

private:
	TACDriveThread*				_tacDriveTrain;
	quint64						_tickCount;
	QByteArray					_currentCommand;


	void triggerElapsed();

	void queueCommand(const QByteArray& command);
};

#endif // TACLITEPROTOCOL_H
