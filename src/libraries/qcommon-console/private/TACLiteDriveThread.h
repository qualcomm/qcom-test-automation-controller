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

#include <string>

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
		if (_ftdiChipset != nullptr)
			_ftdiChipset->setInvertMask(mask);
	}

	virtual void run() override;

	virtual void sendCommand(const std::string& command, bool console = false,
		ReceiveInterface* receiveInterface = nullptr, bool shouldStore = true) override;

	virtual void externalPowerControl(bool state);

	virtual void setPinState(uint16_t pin, bool state) override;
	virtual void sendCommandSequence(CommandEntries& commandEntries) override;

	virtual int getResetCount() override;
	virtual void clearResetCount() override;

	virtual void i2CReadRegister(uint32_t addr, uint32_t reg) override;
	virtual void i2CWriteRegister(uint32_t addr, uint32_t reg, uint32_t data) override;

	// properties
	virtual void setName(const std::string& newName) override;

	// Send Interface
	virtual uint32_t send(const std::string& sendMe, const Arguments& arguments, bool console, ReceiveInterface* recieveInterface, bool store = true) override;
	virtual bool ready() override;

	// Receive Interface
	virtual void receive(FramePackage& framePackage) override;

	std::string portDescription();

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

	virtual void setupConnected() override;
	virtual void setupDiscovery() override;

	bool getElectrialPinValue(uint32_t kCommandHash, const Arguments& arguments);
};

#endif // TACDRIVETRAIN_H
