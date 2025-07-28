#ifndef TacLiteDriveThread_H
#define TacLiteDriveThread_H
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
// Copyright 2013-2019 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "QCommonConsoleGlobal.h"

// libTAC
#include "TACLiteCommand.h"
#include "TACDriveThread.h"
#include "TACLiteProtocol.h"

// QCommon
#include "FTDIChipset.h"
#include "ReceiveInterface.h"

// Qt
#include <QFile>

class QCOMMONCONSOLE_EXPORT TacLiteDriveThread :
	public TACDriveThread
{
Q_OBJECT

public:
	TacLiteDriveThread(uint hash);
	~TacLiteDriveThread();

	void setPinSets(FTDIPinSets pinsets)
	{
		_pinsets = pinsets;
	}

	virtual void run();

	virtual void sendCommand(const QByteArray& command, bool console = false,
		ReceiveInterface* receiveInterface = Q_NULLPTR, bool shouldStore = true);

	virtual void externalPowerControl(bool state);

	virtual void setPinState(quint16 pin, bool state);
	virtual void sendCommandSequence(CommandEntries& commandEntries);

	virtual int getResetCount();
	virtual void clearResetCount();

	virtual void i2CReadRegister(quint32 addr, quint32 reg);
	virtual void i2CWriteRegister(quint32 addr, quint32 reg, quint32 data);

	// properties
	virtual void setName(const QByteArray& newName);

	// Send Interface
	virtual quint32 send(const QByteArray& sendMe, const Arguments& arguments, bool console, ReceiveInterface* recieveInterface, bool store = true);
	virtual bool ready();

	// Receive Interface
	virtual void receive(FramePackage& framePackage);

	QString portDescription();

protected:
	bool openFTDIDevice();

private:
	static bool					_initialized;
	TACLiteProtocol				_tacProtocol;
	bool						_connected{false};
	bool						_readyRead{false};
	FTDIPinSets					_pinsets{eC | eD};
	FTDIChipset					_ftdiChipset;

	void handleIdle(FramePackage& framePackage);
	void handlePlatformID(FramePackage& framePackage);
	void handleSetName(FramePackage& framePackage);
	void handleSetPin(FramePackage& framePackage);
	void handleUUIDResponse(FramePackage& framePackage);
	void handleVersionResponse(FramePackage& framePackage);

	virtual void setupConnected();
	virtual void setupDiscovery();

	bool getElectrialPinValue(quint32 kCommandHash, const Arguments& arguments);
};

#endif // TACDRIVETRAIN_H
