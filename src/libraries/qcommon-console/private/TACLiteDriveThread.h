#ifndef TACLiteDriveThread_H
#define TACLiteDriveThread_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"

#include "TACLiteCommand.h"
#include "TACDriveThread.h"
#include "TACLiteProtocol.h"

// QCommon
#include "FTDIChipset.h"
#include "ReceiveInterface.h"

// Qt
#include <QFile>

class QCOMMONCONSOLE_EXPORT TACLiteDriveThread :
	public TACDriveThread
{
Q_OBJECT

public:
	TACLiteDriveThread(uint hash);
	~TACLiteDriveThread();

	void setPinSets(FTDIPinSets pinsets)
	{
		_pinsets = pinsets;
	}

	void setInvertMask(uint8_t mask)
	{
		if (_ftdiChipset.isNull() == false)
			_ftdiChipset->setInvertMask(mask);
	}

	virtual void run();

	virtual void sendCommand(const QByteArray& command, bool console = false,
		ReceiveInterface* receiveInterface = Q_NULLPTR, bool shouldStore = true);

	virtual void externalPowerControl(bool state);

	virtual void setPinState(uint16_t pin, bool state);
	virtual void sendCommandSequence(CommandEntries& commandEntries);

	virtual int getResetCount();
	virtual void clearResetCount();

	virtual void i2CReadRegister(uint32_t addr, uint32_t reg);
	virtual void i2CWriteRegister(uint32_t addr, uint32_t reg, uint32_t data);

	// properties
	virtual void setName(const QByteArray& newName);

	// Send Interface
	virtual uint32_t send(const QByteArray& sendMe, const Arguments& arguments, bool console, ReceiveInterface* recieveInterface, bool store = true);
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

	bool getElectrialPinValue(uint32_t kCommandHash, const Arguments& arguments);
};

#endif // TACDRIVETRAIN_H
