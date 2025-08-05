#ifndef TACDEVICE_H
#define TACDEVICE_H
/*
	Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries. 
	 
	Redistribution and use in source and binary forms, with or without
	modification, are permitted (subject to the limitations in the
	disclaimer below) provided that the following conditions are met:
	 
		* Redistributions of source code must retain the above copyright
		  notice, this list of conditions and the following disclaimer.
	 
		* Redistributions in binary form must reproduce the above
		  copyright notice, this list of conditions and the following
		  disclaimer in the documentation and/or other materials provided
		  with the distribution.
	 
		* Neither the name of Qualcomm Technologies, Inc. nor the names of its
		  contributors may be used to endorse or promote products derived
		  from this software without specific prior written permission.
	 
	NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
	GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
	HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
	WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
	ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
	GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
	IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
	IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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
