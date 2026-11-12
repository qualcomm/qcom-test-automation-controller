#ifndef USBDESCRIPTORS_H
#define USBDESCRIPTORS_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"

// Common
#include "DebugBoardType.h"
#include "FTDIPinSet.h"
#include "PlatformID.h"

// QT
class QJsonObject;
#include <QList>

#include <cstdint>
#include <string>
#include <vector>

const std::string kUSBDescriptorFileName{"devicelist.json"};

struct QCOMMONCONSOLE_EXPORT USBDescriptor
{
	USBDescriptor()
	{
		for (int i = 0; i < kMaxPinSetCount; i++)
			_pinSets[i] = NoOptions;
	}

	PlatformID					_platformID{0};
	DebugBoardType				_debugBoardType{eUnknownDebugBoard};
	std::string					_name;
	std::string					_description;
	std::string					_usbDescriptor;
	uint32_t					_revision{0};
	std::string					_configurationFilePath;
	FTDIPinSets					_pinSets[kMaxPinSetCount];
};

typedef std::vector<USBDescriptor> DescriptorList;

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
	PlatformID getPlatformId(const std::string& usbDescriptor);

	void addUSBDescriptor(USBDescriptor usbDescriptor);

	bool load(const std::string& filePath);
	bool save(const std::string& filePath);

	std::string getLastError();

private:
	virtual bool read(QJsonObject& parentLevel);
	virtual void write(QJsonObject& parentLevel);

	std::string					_lastError;
	std::string					_modifyDate;
	DescriptorList				_descriptorList;
};

#endif // USBDESCRIPTORS_H
