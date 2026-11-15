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

// Standard C++
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <mutex>
#include <string>

class QCOMMONCONSOLE_EXPORT TACDriveThread :
	public DriveThread,
	public ReceiveInterface
{
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
	bool oldFirmware() const;
	const std::string& firmwareVersion() const;
	uint32_t majorVersion() const;
	uint32_t chipVersion() const;
	uint32_t minorVersion() const;
	uint32_t revisionVersion() const;

	const std::string& name() const;
	virtual void setName(const std::string& newName);

	const std::string& portName() const;
	void setPortName(const std::string& portName);

	const std::string& description() const;
	void setDescription(const std::string& description);

	const std::string& serialNumber() const;
	void setSerialNumber(const std::string& serialNumber);

	const std::string& uuid() const;

	const std::string& macAddress() const;

	// Send Interface
	virtual uint32_t send(const std::string& sendMe, const Arguments& arguments, bool console, ReceiveInterface* recieveInterface, bool store = true) = 0;
	virtual bool ready() = 0;
	virtual bool connected() const { return false; }

	// Receive Interface
	virtual void receive(FramePackage& framePackage) = 0;

	void setThreadDelay(const uint32_t delay);

	// Lifecycle hooks
	virtual void onStopping() override;

	// Direct Event Callback Hooks
	std::function<void(int pin, int state)> onPinStateChanged;
	std::function<void()>                   onDeviceConnected;
	std::function<void()>                   onDeviceDisconnected;
	std::function<void()>                   onDeviceOpen;
	std::function<void(int progress)>       onProgress;
	std::function<void(const std::string&)> onError;
	std::function<void(const std::string&)> onDeviceStatusChange;

protected:
	bool						_waitForCompletion{false};
	std::condition_variable		_completionCv;
	std::mutex					_completionMutex;
	HashType					_hash{0};
	bool						_oldFirmware{false};
	std::string					_portName;
	DebugBoardType				_hardwareType{eUnknownDebugBoard};
	PlatformID					_platformID{MICRO_EPM_BOARD_ID_UNKNOWN};
	std::string					_versionString;
	std::string					_firmwareString;
	uint32_t					_firmwareMajor{0};
	uint32_t					_firmwareChip{0};
	uint32_t					_firmwareMinor{0};
	uint32_t					_firmwareRevision{0};
	std::string					_name;
	std::string                 _macAddress;
	std::string					_description;
	std::string					_uuid;
	std::string					_serialNumber;
	std::string                 _mcnNumber;
	std::string					_helpText;

	int							_resetCount{0};
	uint32_t					_delay{0};
	uint64_t					_lastTimeStamp{0};

	virtual void setupConnected() = 0;
	virtual void setupDiscovery() = 0;

	// logging
	void setupLogging(bool loggingState);
	void shutdownLogging();

	void log(FramePackage& framePackage);
	void timeStampLogMessage(std::string& timeStampMe);
};

#endif // TACDRIVETHREAD_H
