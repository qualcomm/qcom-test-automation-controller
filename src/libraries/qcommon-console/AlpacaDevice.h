#ifndef TACDEVICE_H
#define TACDEVICE_H
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
// Copyright 2024 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/

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
#include <QMutex>
#include <QSharedPointer>

const QByteArray kMACNotSupported{"Not supported"};

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
	static quint32 updateAlpacaDevices();

	static AlpacaDevice findAlpacaDevice(HashType hash);
	static AlpacaDevice findAlpacaDevice(const QByteArray& portName);
	static AlpacaDevice findAlpacaDeviceBySerialNumber(const QByteArray& serialNumber, bool usePartial = true);
	static AlpacaDevice findAlpacaDeviceByDescription(const QByteArray& description, bool usePartial = true);
	static AlpacaDevice findAlpacaDeviceByUSBDescriptor(const QByteArray& descriptor, bool usePartial = true);

	QByteArray getLastError();

	virtual bool open() = 0;
	bool isOpen();
	void close();

	virtual void buildMapping() = 0;
	void buildQuickSettings();

	quint32 commandCount();
	TacCommand commandEntry(quint32 commandIndex);
	TacCommands commandList();

	quint32 quickCommandCount();
	QByteArray getQuickCommand(quint32 index);

	quint32 scriptVariableCount();
	QByteArray getScriptVariable(const quint32 index);
	bool updateScriptVariableValue(const QByteArray& scriptVariable, const QByteArray& value);

	bool getCommandState(const QByteArray& command);
	bool sendCommand(const QByteArray& command, bool state);

	bool quickCommand(const QByteArray& command);

	QByteArray getHelp();

	virtual void setPinState(PinID pin, bool state);

	void setWaitForCompletion();

	bool active();

	HashType hash();

	QByteArray portName() const;
	void setPortName(const QByteArray& portName);

	// properties
	DebugBoardType debugBoardType();
	QString debugBoardTypeString();
	QString hardwareVersionString();
	QString firmwareVersion();

	uint majorVersion();
	QString chipVersion();
	uint minorVersion();
	uint revisionVersion();

	QByteArray description();
	QByteArray usbDescriptor();
	QByteArray serialNumber();
	PlatformID platformID();
	QByteArray macAddress();
	PlatformConfiguration platformConfiguration();

	void externalPowerControl(bool state);

	QByteArray name() const;
	void setName(const QByteArray& newName);

	QSize windowDimension();

	QString description() const;
	void setDescription(const QString& description);

	QString serialNumber() const;
	void setSerialNumber(QString serialNumber);

	QString uuid();

	int getResetCount();
	void clearResetCount();

	void i2CReadRegister(quint32 addr, quint32 reg);
	void i2CWriteRegister(quint32 addr, quint32 reg, quint32 data);

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
	static QMutex				_mutex;
	static AlpacaDevices		_alpacaDevices;

	bool						_active{false};
	HashType					_hash{0};
	DebugBoardType				_boardType{eUnknownDebugBoard};
	QByteArray					_portName;
	QByteArray					_description;
	QByteArray					_usbDescriptor;
	QByteArray                  _macAddress{kMACNotSupported};
	QByteArray					_serialNumber;
	uint						_chipVersion{0};
	PlatformID					_platformID{ALPACA_LITE_ID};
	QByteArray					_helpText;
	TACDriveThread*				_driveThread{Q_NULLPTR};

	PlatformConfiguration		_platformConfiguration;
	TacCommandMap				_commands;
	TacCommands					_commandList;
	QList<QByteArray>			_quickCommandList;
	AlpacaScript				_alpacaScript;
	QByteArray                  _lastError;
};

#endif // TACDEVICE_H
