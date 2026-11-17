// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef TACPSOCDRIVETHREAD_H
#define TACPSOCDRIVETHREAD_H

#include "QCommonConsoleGlobal.h"

#include "TACDriveThread.h"
#include "TACPSOCProtocol.h"

#include "SerialPort.h"
#include "SerialPortInfo.h"

#include <string>

class QCOMMONCONSOLE_EXPORT TACPSOCDriveThread :
	public TACDriveThread
{
public:
	TACPSOCDriveThread(uint32_t hash);
	~TACPSOCDriveThread();

	virtual void run() override;

	virtual void sendCommand(const std::string& command, bool console = false,
		ReceiveInterface* receiveInterface = nullptr, bool shouldStore = true) override;
	void endTransaction(ReceiveInterface* receiveInterface);

	virtual void setPinState(uint16_t pin, bool state) override;
	virtual void setAddressPinState(const std::string& i2cAddress, uint16_t pin, bool state) override;
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
	virtual bool connected() const override { return _connected; }

	// Receive Interface
	virtual void receive(FramePackage& framePackage) override;

protected:
	bool openSerialDevice();

	void handleError(SerialPortError error);

private:
	std::string serialPortError();
	bool readSerialData();

	bool						_connected{false};
	TACPSOCProtocol				_tacProtocol;
	SerialPortInfo				_tacPortInfo;
	SerialPort*					_serialPort{nullptr};

	void handleGetNameResponse(FramePackage& framePackage);
	void handleGetResetCount(FramePackage& framePackage);
	void handleI2CRead(FramePackage& framePackage);
	void handleI2CWrite(FramePackage& framePackage);
	void handleIdle(FramePackage& framePackage);
	void handleSetPin(FramePackage& framePackage);
	void handlePlatformID(FramePackage& framePackage);
	void handleSetName(FramePackage& framePackage);
	void handleUUIDResponse(FramePackage& framePackage);
	void handleVersionResponse(FramePackage& framePackage);

	virtual void setupConnected() override;
	virtual void setupDiscovery() override;

	void log(FramePackage& framePackage);
};

#endif // TACPSOCDRIVETHREAD_H
