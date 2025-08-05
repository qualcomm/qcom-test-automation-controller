#ifndef USBDESCRIPTORS_H
#define USBDESCRIPTORS_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson
*/

// libTAC
#include "QCommonConsoleGlobal.h"

// Common
#include "DebugBoardType.h"
#include "FTDIPinSet.h"
#include "PlatformID.h"

// QT
class QJsonObject;
#include <QList>
#include <QString>

const QString kUSBDescriptorFileName{QStringLiteral("devicelist.json")};

struct QCOMMONCONSOLE_EXPORT USBDescriptor
{
	USBDescriptor()
	{
		for (int i = 0; i < kMaxPinSetCount; i++)
			_pinSets[i] = NoOptions;
	}

	PlatformID					_platformID{0};
	DebugBoardType				_debugBoardType{eUnknownDebugBoard};
	QString						_name;
	QString						_description;
	QByteArray					_usbDescriptor;
	quint32						_revision{0};
	QByteArray					_configurationFilePath;
	FTDIPinSets					_pinSets[kMaxPinSetCount];
};

typedef QList<USBDescriptor> DescriptorList;

class QCOMMONCONSOLE_EXPORT USBDescriptors
{
public:
	USBDescriptors();
	USBDescriptors(const USBDescriptors&) = delete;
	USBDescriptors& operator=(const USBDescriptors&) = delete;

	~USBDescriptors() = default;

	QList<PlatformID> getPlatforms();

	DescriptorList getDescriptors();
	DescriptorList getDescriptorsByBoardType(DebugBoardType debugBoardType);

	USBDescriptor getPlatformDescriptor(PlatformID platformID);
	PlatformID getPlatformId(const QByteArray& usbDescriptor);

	void addUSBDescriptor(USBDescriptor usbDescriptor);

	bool load(const QString& filePath);
	bool save(const QString& filePath);

	QString getLastError();

private:
	virtual bool read(QJsonObject& parentLevel);
	virtual void write(QJsonObject& parentLevel);

	QString						_lastError;
	QString						_modifyDate;
	DescriptorList				_descriptorList;
};

#endif // USBDESCRIPTORS_H
