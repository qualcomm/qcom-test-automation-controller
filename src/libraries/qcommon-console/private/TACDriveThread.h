// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef TACDRIVETHREAD_H
#define TACDRIVETHREAD_H

#include "QCommonConsoleGlobal.h"

// libTAC
#include "DebugBoardType.h"
#include "Notification.h"

// QCommon
#include "AlpacaScript.h"
#include "DriveThread.h"
#include "PlatformID.h"
#include "ReceiveInterface.h"
#include "StringUtilities.h"

// Qt
#include <QCoreApplication>

#include <cstdint>
#include <string>

class QCOMMONCONSOLE_EXPORT TACDriveThread :
	public DriveThread,
	public ReceiveInterface
{
Q_OBJECT

public:
	TACDriveThread(HashType hash);
	~TACDriveThread();

	bool resetLogging();

	static TACDriveThread* openPort(const std::string& portName);

	HashType hash();

	void waitForCompletion();
	void setWaitForCompletion();
	void clearWaitForCompletion();
	bool waitForCompletionStatus();

	std::string decodeCommand(const std::string& command, Arguments& arg);
	bool checkLocalStore(FramePackage& framePackage);

	virtual void sendCommand(const std::string& command, bool console = false,
		ReceiveInterface* receiveInterface = nullptr, bool shouldStore = true) = 0;

	virtual void setPinState(uint16_t pin, bool state) = 0;
	virtual void setAddressPinState(const std::string& i2cAddress, uint16_t pin, bool state) {}
	virtual void sendCommandSequence(CommandEntries& commandEntries) = 0;

	virtual int getResetCount() = 0;
	virtual void clearResetCount() = 0;

	virtual void i2CReadRegister(uint32_t addr, uint32_t reg) = 0;
	virtual void i2CWriteRegister(uint32_t addr, uint32_t reg, uint32_t data) = 0;

	// properties
	DebugBoardType debugBoardType();
	std::string debugBoardTypeString();

	PlatformID platformID();
	virtual std::string hardwareVersionString();

	// firmware properties
	bool oldFirmware();
	std::string firmwareVersion();
	uint majorVersion();
	uint chipVersion();
	uint minorVersion();
	uint revisionVersion();

	std::string name() const;
	void setName(const std::string& newName);

	std::string portName() const;
	void setPortName(const std::string& portName);

	std::string description() const;
	void setDescription(const std::string& description);

	std::string serialNumber() const;
	void setSerialNumber(const std::string& serialNumber);

	std::string uuid();

	std::string macAddress();

	// Send Interface
	virtual uint32_t send(const std::string& sendMe, const Arguments& arguments, bool console, ReceiveInterface* recieveInterface, bool store = true) = 0;
	virtual bool ready() = 0;

	// Receive Interface
	virtual void receive(FramePackage& framePackage) = 0;

	void setThreadDelay(const uint delay);

signals:
	void pinStateChanged(uint64_t pin, bool state);

	void transactionEnded();
	void progress(const uint8_t value=10, NotificationLevel level=eInfoNotification);

	void deviceOpen();
	void errorOnOpen(const std::string& errorString);

	void deviceStatusChange(const std::string& deviceStatus);

	void hardwareTypeUpdate(const std::string& hardwareType);
	void hardwareVersionUpdate(const std::string& hardwareVersion);
	void firmwareVersionUpdate(const std::string& firmwareVersion);
	void nameUpdate(const std::string& name);
	void uuidUpdate(const std::string& uuid);
	void serialNumUpdate(const std::string& serialNumber);
	void platformIDUpdate(int platformID);

	void deviceConnected();
	void deviceDisconnected();

	void resetCountCleared();
	void resetCountUpdate(uint32_t resetCount);

	void i2CReadResult(const std::string& readResult, bool valid);
	void i2CWriteResult(const std::string& readResult);

protected:
	bool						_waitForCompletion{false};
	HashType					_hash{0};
	bool						_oldFirmware{false};
	std::string					_portName;
	DebugBoardType				_hardwareType{eUnknownDebugBoard};
	PlatformID					_platformID{MICRO_EPM_BOARD_ID_UNKNOWN};
	std::string					_versionString;
	std::string					_firmwareString;
	uint						_firmwareMajor{0};
	uint						_firmwareChip{0};
	uint						_firmwareMinor{0};
	uint						_firmwareRevision{0};
	std::string					_name;
	std::string                 _macAddress;
	std::string					_description;
	std::string					_uuid;
	std::string					_serialNumber;
	std::string                 _mcnNumber;
	std::string					_helpText;

	int							_resetCount{0};
	uint						_delay{0};

	virtual void setupConnected() = 0;
	virtual void setupDiscovery() = 0;

	// logging
	void setupLogging(bool loggingState);
	void shutdownLogging();

	void log(FramePackage& framePackage);
	void timeStampLogMessage(std::string& timeStampMe);
};

#endif // TACDRIVETHREAD_H
