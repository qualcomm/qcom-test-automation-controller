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

	QByteArray decodeCommand(const QByteArray& command, Arguments& arg);
	bool checkLocalStore(FramePackage& framePackage);

	virtual void sendCommand(const QByteArray& command, bool console = false,
		ReceiveInterface* receiveInterface = Q_NULLPTR, bool shouldStore = true) = 0;

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

	QByteArray name() const;
	void setName(const QByteArray& newName);

	QByteArray portName() const;
	void setPortName(const QByteArray& portName);

	std::string description() const;
	void setDescription(const std::string& description);

	QString serialNumber() const;
	void setSerialNumber(QString serialNumber);

	QString uuid();

	std::string macAddress();

	// Send Interface
	virtual uint32_t send(const QByteArray& sendMe, const Arguments& arguments, bool console, ReceiveInterface* recieveInterface, bool store = true) = 0;
	virtual bool ready() = 0;

	// Receive Interface
	virtual void receive(FramePackage& framePackage) = 0;

	void setThreadDelay(const uint delay);

signals:
	void pinStateChanged(/*PinID*/ quint64 pin, bool state);

	void transactionEnded();
	void progress(const quint8 value=10, NotificationLevel level=eInfoNotification);

	void deviceOpen();
	void errorOnOpen(const QByteArray& errorString);

	void deviceStatusChange(const QString& deviceStatus);

	void hardwareTypeUpdate(const QString& hardwareType);
	void hardwareVersionUpdate(const QString& hardwareVersion);
	void firmwareVersionUpdate(const QString& firmwareVersion);
	void nameUpdate(const QString& name);
	void uuidUpdate(const QString& uuid);
	void serialNumUpdate(const QString& serialNumber);
	void platformIDUpdate(int platformID);

	void deviceConnected();
	void deviceDisconnected();

	void resetCountCleared();
	void resetCountUpdate(quint32 resetCount);

	void i2CReadResult(const QByteArray& readResult, bool valid);
	void i2CWriteResult(const QByteArray& readResult);

protected:
	bool						_waitForCompletion{false};
	HashType					_hash{0};
	bool						_oldFirmware{false};
	QByteArray					_portName;
	DebugBoardType				_hardwareType{eUnknownDebugBoard};
	PlatformID					_platformID{MICRO_EPM_BOARD_ID_UNKNOWN};
	QByteArray					_versionString;
	std::string					_firmwareString;
	uint						_firmwareMajor{0};
	uint						_firmwareChip{0};
	uint						_firmwareMinor{0};
	uint						_firmwareRevision{0};
	QByteArray					_name;
	std::string                 _macAddress;
	std::string					_description;
	QString						_uuid;
	QString						_serialNumber;
	std::string                 _mcnNumber;
	QString						_helpText;

	int							_resetCount{0};
	uint						_delay{0};

	virtual void setupConnected() = 0;
	virtual void setupDiscovery() = 0;

	// logging
	void setupLogging(bool loggingState);
	void shutdownLogging();

	void log(FramePackage& framePackage);
	void timeStampLogMessage(QString& timeStampMe);
};

#endif // TACDRIVETHREAD_H
