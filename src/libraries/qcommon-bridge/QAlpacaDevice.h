#ifndef QALPACADEVICE_H
#define QALPACADEVICE_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonBridgeGlobal.h"

// QCommonConsole
#include "AlpacaDevice.h"
#include "Notification.h"

// Qt
#include <QObject>
#include <QSize>
#include <QString>

class QCOMMONBRIDGE_EXPORT QAlpacaDevice : public QObject
{
	Q_OBJECT

public:
	explicit QAlpacaDevice(AlpacaDevice device, QObject* parent = nullptr);
	virtual ~QAlpacaDevice();

	AlpacaDevice rawDevice() const { return _device; }
	bool isValid() const { return _device != nullptr; }

	QString name() const;
	void setName(const QString& newName);

	QString serialNumber() const;
	QString portName() const;
	QString description() const;
	QString usbDescriptor() const;
	QString hardwareVersionString() const;
	QString firmwareVersion() const;
	QString chipVersion() const;
	QString uuid() const;
	QString macAddress() const;

	uint64_t hash() const { return _device ? _device->hash() : 0; }
	PlatformID platformID() const { return _device ? _device->platformID() : 0; }

	PlatformConfiguration platformConfiguration() const;

	bool isOpen() const;
	bool open();
	void close();

	void setPinState(uint64_t pin, bool state);
	void setAddressPinState(const QString& address, int pin, bool state);
	void quickCommand(const QString& command);

	bool sendCommand(const QString& command, bool state);
	bool getCommandState(const QString& command);

	bool updateScriptVariableValue(const QString& variable, const QString& value);

	QSize windowDimension() const;
	QString getLastError() const;

	static AlpacaDevice findAlpacaDevice(const QString& portName);
	static AlpacaDevices getAlpacaDevices(DebugBoardType type = eUnknownDebugBoard);

signals:
	void pinStateChanged(quint64 pin, bool state);
	void deviceDisconnected();
	void progress(const quint8 value = 10, NotificationLevel level = eInfoNotification);

private:
	void handlePinStateChanged(int pin, int state);
	void handleDeviceDisconnected();
	void handleProgress(int value);

	AlpacaDevice _device;
};

#endif // QALPACADEVICE_H
