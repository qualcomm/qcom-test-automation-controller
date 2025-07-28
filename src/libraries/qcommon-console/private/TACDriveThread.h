#ifndef TACDRIVETHREAD_H
#define TACDRIVETHREAD_H
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
// Copyright 2013-2023 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "QCommonConsoleGlobal.h"

// libTAC
#include "DebugBoardType.h"
#include "Notification.h"

// QCommon
#include "AlpacaScript.h"
#include "AppCore.h"
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

	static TACDriveThread* openPort(const QByteArray& portName);

	HashType hash()
	{
		return _hash;
	}

	void waitForCompletion()
	{
		int count{0};

		while (_waitForCompletion)
		{
			QThread::msleep(100);				// Give our high priority time to process
			QCoreApplication::processEvents();  // Give the UI a timeslice to update

			if (count++ > 50)
			{
				AppCore::writeToApplicationLogLine("Wait for completion timed out.");

				_waitForCompletion = false;
			}
		}
	}

	void setWaitForCompletion()
	{
		_waitForCompletion = true;
	}

	void clearWaitForCompletion()
	{
		_waitForCompletion = false;
	}

	bool oldFirmware()
	{
		return _oldFirmware;
	}

	QByteArray decodeCommand(const QByteArray& command, Arguments& arg);
	bool checkLocalStore(FramePackage& framePackage);

	virtual void sendCommand(const QByteArray& command, bool console = false,
		ReceiveInterface* receiveInterface = Q_NULLPTR, bool shouldStore = true) = 0;

	virtual void setPinState(quint16 pin, bool state) = 0;
	virtual void sendCommandSequence(CommandEntries& commandEntries) = 0;

	virtual int getResetCount() = 0;
	virtual void clearResetCount() = 0;

	virtual void i2CReadRegister(quint32 addr, quint32 reg) = 0;
	virtual void i2CWriteRegister(quint32 addr, quint32 reg, quint32 data) = 0;

	// properties
	DebugBoardType debugBoardType();
	QString debugBoardTypeString();

	PlatformID platformID();
	virtual QString hardwareVersionString();

	QString firmwareVersion();

	uint majorVersion()
	{
		return _firmwareMajor;
	}

	uint chipVersion()
	{
		return _firmwareChip;
	}

	uint minorVersion()
	{
		return _firmwareMinor;
	}

	uint revisionVersion()
	{
		return _firmwareRevision;
	}

	QByteArray name() const
	{
		return _name;
	}
	void setName(const QByteArray& newName)
	{
		_name = newName;
	}

	QByteArray portName() const
	{
		return _portName;
	}

	void setPortName(const QByteArray& portName)
	{
		_portName = portName;
	}

	QString description() const
	{
		return _description;
	}
	void setDescription(const QString& description)
	{
		_description = description;
	}

	QString serialNumber() const
	{
		return _serialNumber;
	}
	void setSerialNumber(QString serialNumber)
	{
		_serialNumber = serialNumber;
	}

	QString uuid();

	QByteArray macAddress();

	// Send Interface
	virtual quint32 send(const QByteArray& sendMe, const Arguments& arguments, bool console, ReceiveInterface* recieveInterface, bool store = true) = 0;
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
	PlatformID					_platformID{ALPACA_LITE_ID};
	QByteArray					_versionString;
	QByteArray					_firmwareString;
	uint						_firmwareMajor{0};
	uint						_firmwareChip{0};
	uint						_firmwareMinor{0};
	uint						_firmwareRevision{0};
	QByteArray					_name;
	QByteArray                  _macAddress;
	QString						_description;
	QString						_uuid;
	QString						_serialNumber;
	QString                     _mcnNumber;
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
