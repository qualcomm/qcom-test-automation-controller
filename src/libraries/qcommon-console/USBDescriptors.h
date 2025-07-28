#ifndef USBDESCRIPTORS_H
#define USBDESCRIPTORS_H
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
// Copyright 2022 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
