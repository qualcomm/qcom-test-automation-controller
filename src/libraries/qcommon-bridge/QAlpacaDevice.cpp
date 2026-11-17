// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QAlpacaDevice.h"
#include "QCommonTypes.h"

QAlpacaDevice::QAlpacaDevice(AlpacaDevice device, QObject* parent) :
	QObject(parent),
	_device(device)
{
	if (_device)
	{
		_device->onPinStateChanged = std::bind_front(&QAlpacaDevice::handlePinStateChanged, this);
		_device->onDeviceDisconnected = std::bind_front(&QAlpacaDevice::handleDeviceDisconnected, this);
		_device->onProgress = std::bind_front(&QAlpacaDevice::handleProgress, this);
	}
}

void QAlpacaDevice::handlePinStateChanged(int pin, int state)
{
	QMetaObject::invokeMethod(this, &QAlpacaDevice::pinStateChanged, Qt::QueuedConnection,
	                          static_cast<quint64>(pin), state != 0);
}

void QAlpacaDevice::handleDeviceDisconnected()
{
	QMetaObject::invokeMethod(this, &QAlpacaDevice::deviceDisconnected, Qt::QueuedConnection);
}

void QAlpacaDevice::handleProgress(int value)
{
	QMetaObject::invokeMethod(this, &QAlpacaDevice::progress, Qt::QueuedConnection,
	                          static_cast<quint8>(value), eInfoNotification);
}

QAlpacaDevice::~QAlpacaDevice()
{
	if (_device)
	{
		_device->onPinStateChanged = nullptr;
		_device->onDeviceDisconnected = nullptr;
		_device->onProgress = nullptr;
	}
}

QString QAlpacaDevice::name() const
{
	return _device ? toQString(_device->name()) : QString();
}

void QAlpacaDevice::setName(const QString& newName)
{
	if (_device)
		_device->setName(toStdString(newName));
}

QString QAlpacaDevice::serialNumber() const
{
	return _device ? toQString(_device->serialNumber()) : QString();
}

QString QAlpacaDevice::portName() const
{
	return _device ? toQString(_device->portName()) : QString();
}

QString QAlpacaDevice::description() const
{
	return _device ? toQString(_device->description()) : QString();
}

QString QAlpacaDevice::usbDescriptor() const
{
	return _device ? toQString(_device->usbDescriptor()) : QString();
}

QString QAlpacaDevice::hardwareVersionString() const
{
	return _device ? toQString(_device->hardwareVersionString()) : QString();
}

QString QAlpacaDevice::firmwareVersion() const
{
	return _device ? toQString(_device->firmwareVersion()) : QString();
}

QString QAlpacaDevice::chipVersion() const
{
	return _device ? toQString(_device->chipVersion()) : QString();
}

QString QAlpacaDevice::uuid() const
{
	return _device ? toQString(_device->uuid()) : QString();
}

QString QAlpacaDevice::macAddress() const
{
	return _device ? toQString(_device->macAddress()) : QString();
}

void QAlpacaDevice::setPinState(uint64_t pin, bool state)
{
	if (_device)
		_device->setPinState(pin, state);
}

void QAlpacaDevice::setAddressPinState(const QString& address, int pin, bool state)
{
	if (_device)
		_device->setAddressPinState(toStdString(address), pin, state);
}

void QAlpacaDevice::quickCommand(const QString& command)
{
	if (_device)
		_device->quickCommand(toStdString(command));
}

PlatformConfiguration QAlpacaDevice::platformConfiguration() const
{
	return _device ? _device->platformConfiguration() : PlatformConfiguration();
}

bool QAlpacaDevice::isOpen() const
{
	return _device ? _device->isOpen() : false;
}

bool QAlpacaDevice::open()
{
	return _device ? _device->open() : false;
}

void QAlpacaDevice::close()
{
	if (_device)
		_device->close();
}

bool QAlpacaDevice::sendCommand(const QString& command, bool state)
{
	return _device ? _device->sendCommand(toStdString(command), state) : false;
}

bool QAlpacaDevice::getCommandState(const QString& command)
{
	return _device ? _device->getCommandState(toStdString(command)) : false;
}

bool QAlpacaDevice::updateScriptVariableValue(const QString& variable, const QString& value)
{
	return _device ? _device->updateScriptVariableValue(toStdString(variable), toStdString(value)) : false;
}

QSize QAlpacaDevice::windowDimension() const
{
	if (_device)
	{
		auto dim = _device->windowDimension();
		return QSize(dim.first, dim.second);
	}
	return QSize(555, 725);
}

QString QAlpacaDevice::getLastError() const
{
	return _device ? toQString(_device->getLastError()) : QString();
}

AlpacaDevice QAlpacaDevice::findAlpacaDevice(const QString& portName)
{
	return _AlpacaDevice::findAlpacaDevice(toStdString(portName));
}

AlpacaDevices QAlpacaDevice::getAlpacaDevices(DebugBoardType type)
{
	AlpacaDevices devs;
	_AlpacaDevice::getAlpacaDevices(devs, type);
	return devs;
}
