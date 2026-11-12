// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef TACDEVICE_H
#define TACDEVICE_H

#include "QCommonConsoleGlobal.h"

// libTAC
#include "DebugBoardType.h"
#include "PlatformConfiguration.h"
#include "TACCommand.h"
#include "private/TACDriveThread.h"

// QCommon
#include "AlpacaScript.h"
#include "PlatformID.h"
#include "StringUtilities.h"

// Qt
#include <QList>
#include <QSharedPointer>

#include <mutex>
#include <string>
#include <vector>

const std::string kMACNotSupported{"Not supported"};

class _AlpacaDevice;

typedef QSharedPointer<_AlpacaDevice> AlpacaDevice;
typedef QList<AlpacaDevice> AlpacaDevices;

class QCOMMONCONSOLE_EXPORT _AlpacaDevice:
	public QObject
{
Q_OBJECT

public:
	_AlpacaDevice() = default;
	virtual ~_AlpacaDevice();

	static void getAlpacaDevices(AlpacaDevices& alpacaDevices, DebugBoardType debugBoardTypeFilter = eUnknownDebugBoard);
	static uint32_t updateAlpacaDevices();

	static AlpacaDevice findAlpacaDevice(HashType hash);
	static AlpacaDevice findAlpacaDevice(const std::string& portName);
	static AlpacaDevice findAlpacaDeviceBySerialNumber(const std::string& serialNumber, bool usePartial = true);
	static AlpacaDevice findAlpacaDeviceByDescription(const std::string& description, bool usePartial = true);
	static AlpacaDevice findAlpacaDeviceByUSBDescriptor(const std::string& descriptor, bool usePartial = true);

	std::string getLastError();

	virtual bool open() = 0;
	bool isOpen();
	void close();

	virtual void buildMapping() = 0;
	void buildQuickSettings();

	uint32_t commandCount();
	TACCommand commandEntry(uint32_t commandIndex);
	TACCommands commandList();

	uint32_t quickCommandCount();
	std::string getQuickCommand(uint32_t index);

	uint32_t scriptVariableCount();
	std::string getScriptVariable(const uint32_t index);
	bool updateScriptVariableValue(const std::string& scriptVariable, const std::string& value);

	bool getCommandState(const std::string& command);
	bool sendCommand(const std::string& command, bool state);

	bool quickCommand(const std::string& command);

	bool isCommandQueueClear();

	std::string getHelp();

	virtual void setPinState(PinID pin, bool state);
	virtual void setAddressPinState(const std::string& i2cAddress, uint16_t pin, bool state);

	void setWaitForCompletion();

	bool active();

	HashType hash();

	std::string portName() const;
	void setPortName(const std::string& portName);

	// properties
	DebugBoardType debugBoardType();
	QString debugBoardTypeString();
	QString hardwareVersionString();
	QString firmwareVersion();

	uint majorVersion();
	std::string chipVersion();
	uint minorVersion();
	uint revisionVersion();

	std::string description();
	std::string usbDescriptor();
	std::string serialNumber();
	PlatformID platformID();
	std::string macAddress();
	PlatformConfiguration platformConfiguration();

	void externalPowerControl(bool state);

	std::string name() const;
	void setName(const std::string& newName);

	QSize windowDimension();

	std::string description() const;
	void setDescription(const std::string& description);

	QString serialNumber() const;
	void setSerialNumber(QString serialNumber);

	QString uuid();

	int getResetCount();
	void clearResetCount();

	void i2CReadRegister(uint32_t addr, uint32_t reg);
	void i2CWriteRegister(uint32_t addr, uint32_t reg, uint32_t data);

	void setActive(bool active = true)
	{
		_active = active;
	}

signals:
	void pinStateChanged(/*PinID*/ quint64 pin, bool state);
	void progress(const quint8 value=10, NotificationLevel level=eInfoNotification);
	void errorEvent(const QByteArray& message);

public slots:
	void on_pinStateChanged(/*PinID*/ quint64 pin, bool state);
	void onDeviceDisconnect();

protected:
	static std::mutex			_mutex;
	static AlpacaDevices		_alpacaDevices;

	bool						_active{false};
	HashType					_hash{0};
	DebugBoardType				_boardType{eUnknownDebugBoard};
	std::string					_portName;
	std::string					_description;
	std::string					_usbDescriptor;
	std::string                 _macAddress{kMACNotSupported};
	std::string					_serialNumber;
	uint						_chipVersion{0};
	PlatformID					_platformID{MICRO_EPM_BOARD_ID_UNKNOWN};
	std::string					_helpText;
	TACDriveThread*				_driveThread{Q_NULLPTR};

	PlatformConfiguration		_platformConfiguration;
	TACCommandMap				_commands;
	TACCommands					_commandList;
	std::vector<std::string>	_quickCommandList;
	AlpacaScript				_alpacaScript;
	std::string                  _lastError;
};

#endif // TACDEVICE_H
