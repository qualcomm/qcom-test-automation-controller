#ifndef QUSBDESCRIPTORS_H
#define QUSBDESCRIPTORS_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonBridgeGlobal.h"

// QCommonConsole
#include "DebugBoardType.h"
#include "FTDIPinSet.h"
#include "PlatformID.h"
#include "USBDescriptors.h"

// Qt
#include <QByteArray>
#include <QList>
#include <QString>

struct QCOMMONBRIDGE_EXPORT QUSBDescriptor
{
	QUSBDescriptor();
	QUSBDescriptor(const USBDescriptor& d);
	USBDescriptor toStd() const;

	PlatformID					_platformID{0};
	DebugBoardType				_debugBoardType{eUnknownDebugBoard};
	QString						_name;
	QString						_description;
	QByteArray					_usbDescriptor;
	quint32						_revision{0};
	QString						_configurationFilePath;
	FTDIPinSets					_pinSets[kMaxPinSetCount];
};

typedef QList<QUSBDescriptor> QDescriptorList;

class QCOMMONBRIDGE_EXPORT QUSBDescriptors
{
public:
	QUSBDescriptors() = default;
	~QUSBDescriptors() = default;

	QList<PlatformID> getPlatforms();
	QDescriptorList getDescriptors();
	QDescriptorList getDescriptorsByBoardType(DebugBoardType debugBoardType);
	QUSBDescriptor getPlatformDescriptor(PlatformID platformID);
	PlatformID getPlatformId(const QByteArray& usbDescriptor);
	void addUSBDescriptor(const QUSBDescriptor& usbDescriptor);

	bool load(const QString& filePath);
	bool save(const QString& filePath);

	QString getLastError() const;

private:
	USBDescriptors _impl;
};

#endif // QUSBDESCRIPTORS_H
