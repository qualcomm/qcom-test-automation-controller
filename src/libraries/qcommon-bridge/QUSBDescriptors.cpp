// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QUSBDescriptors.h"
#include "QCommonTypes.h"

QUSBDescriptor::QUSBDescriptor()
{
	for (int i = 0; i < kMaxPinSetCount; i++)
		_pinSets[i] = NoOptions;
}

QUSBDescriptor::QUSBDescriptor(const USBDescriptor& d) :
	_platformID(d._platformID),
	_debugBoardType(d._debugBoardType),
	_name(toQString(d._name)),
	_description(toQString(d._description)),
	_usbDescriptor(QByteArray::fromStdString(d._usbDescriptor)),
	_revision(d._revision),
	_configurationFilePath(toQString(d._configurationFilePath))
{
	for (int i = 0; i < kMaxPinSetCount; i++)
		_pinSets[i] = d._pinSets[i];
}

USBDescriptor QUSBDescriptor::toStd() const
{
	USBDescriptor d;
	d._platformID = _platformID;
	d._debugBoardType = _debugBoardType;
	d._name = toStdString(_name);
	d._description = toStdString(_description);
	d._usbDescriptor = std::string(_usbDescriptor.constData(), static_cast<size_t>(_usbDescriptor.size()));
	d._revision = _revision;
	d._configurationFilePath = toStdString(_configurationFilePath);
	for (int i = 0; i < kMaxPinSetCount; i++)
		d._pinSets[i] = _pinSets[i];
	return d;
}

QList<PlatformID> QUSBDescriptors::getPlatforms()
{
	std::vector<PlatformID> platforms = _impl.getPlatforms();
	QList<PlatformID> result;
	result.reserve(static_cast<qsizetype>(platforms.size()));
	for (const auto& pid : platforms)
		result.append(pid);
	return result;
}

QDescriptorList QUSBDescriptors::getDescriptors()
{
	DescriptorList list = _impl.getDescriptors();
	QDescriptorList result;
	result.reserve(static_cast<qsizetype>(list.size()));
	for (const auto& d : list)
		result.append(QUSBDescriptor(d));
	return result;
}

QDescriptorList QUSBDescriptors::getDescriptorsByBoardType(DebugBoardType debugBoardType)
{
	DescriptorList list = _impl.getDescriptorsByBoardType(debugBoardType);
	QDescriptorList result;
	result.reserve(static_cast<qsizetype>(list.size()));
	for (const auto& d : list)
		result.append(QUSBDescriptor(d));
	return result;
}

QUSBDescriptor QUSBDescriptors::getPlatformDescriptor(PlatformID platformID)
{
	return QUSBDescriptor(_impl.getPlatformDescriptor(platformID));
}

PlatformID QUSBDescriptors::getPlatformId(const QByteArray& usbDescriptor)
{
	return _impl.getPlatformId(std::string(usbDescriptor.constData(), static_cast<size_t>(usbDescriptor.size())));
}

void QUSBDescriptors::addUSBDescriptor(const QUSBDescriptor& usbDescriptor)
{
	_impl.addUSBDescriptor(usbDescriptor.toStd());
}

bool QUSBDescriptors::load(const QString& filePath)
{
	return _impl.load(toStdString(filePath));
}

bool QUSBDescriptors::save(const QString& filePath)
{
	return _impl.save(toStdString(filePath));
}

QString QUSBDescriptors::getLastError() const
{
	return toQString(const_cast<USBDescriptors&>(_impl).getLastError());
}
