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

	void setInvertMask(quint8 mask)
	{
		if (_ftdiChipset.isNull() == false)
			_ftdiChipset->setInvertMask(mask);
	}

	virtual void run() override;

	virtual void sendCommand(const QByteArray& command, bool console = false,
		ReceiveInterface* receiveInterface = Q_NULLPTR, bool shouldStore = true) override;

	virtual void externalPowerControl(bool state);

	virtual bool setPinState(quint16 pin, bool state) override;
	virtual void sendCommandSequence(CommandEntries& commandEntries) override;

	virtual int getResetCount() override;
	virtual void clearResetCount() override;

	virtual void i2CReadRegister(quint32 addr, quint32 reg) override;
	virtual void i2CWriteRegister(quint32 addr, quint32 reg, quint32 data) override;

	// properties
	virtual void setName(const QByteArray& newName) override;

	// Send Interface
	virtual quint32 send(const QByteArray& sendMe, const Arguments& arguments, bool console, ReceiveInterface* recieveInterface, bool store = true) override;
	virtual bool ready() override;

	// Receive Interface
	virtual void receive(FramePackage& framePackage) override;

	QString portDescription();

protected:
	bool openFTDIDevice();
	bool performTransportReset() override;

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

	virtual void setupConnected() override;
	virtual void setupDiscovery() override;

	bool getElectrialPinValue(quint32 kCommandHash, const Arguments& arguments);
};

#endif // TACDRIVETRAIN_H
