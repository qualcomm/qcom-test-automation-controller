#ifndef TACSTM32DRIVETHREAD_H
#define TACSTM32DRIVETHREAD_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"

#include "TACDriveThread.h"
#include "TACSTM32Protocol.h"

// QCommon
#include "ReceiveInterface.h"

#include <string>

class STM32Device;

class QCOMMONCONSOLE_EXPORT TACSTM32DriveThread :
	public TACDriveThread
{
Q_OBJECT

public:
	TACSTM32DriveThread(uint hash);
	~TACSTM32DriveThread();

	virtual void run() override;

	virtual void sendCommand(const std::string& command, bool console = false,
		ReceiveInterface* receiveInterface = nullptr, bool shouldStore = true) override;

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

protected:
	bool openSTM32Device();

private:
	bool						_connected{false};
	STM32Device*				_stm32Device{nullptr};

	TACSTM32Protocol			_tacProtocol;

	virtual void setupConnected() override;
	virtual void setupDiscovery() override;
};

#endif // TACSTM32DRIVETHREAD_H
