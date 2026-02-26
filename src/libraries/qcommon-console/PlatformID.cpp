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
// Copyright 2021 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "PlatformID.h"

// QCommon
#include "AppCore.h"
#include "ConsoleApplicationEnhancements.h"
#include "RangedContainer.h"
#include "USBDescriptors.h"

const PlatformID MICRO_EPM_BOARD_ID_SPMV4{1};
const PlatformID MICRO_EPM_BOARD_ID_EPMV4{2};
const PlatformID MICRO_EPM_BOARD_ID_ALPACA{3};
const PlatformID MICRO_EPM_BOARD_ID_MICROEPM_TAC{4};
const PlatformID MICRO_EPM_BOARD_ID_ALPACA_V2{5};
const PlatformID MICRO_EPM_BOARD_ID_ALPACA_V3{6};
const PlatformID MICRO_EPM_BOARD_ID_ALPACA_V3P1{7};
const PlatformID EPM_BOARD_ID_MTP_V3P2{8};
const PlatformID EPM_BOARD_ID_QRD_V1P0{9};
const PlatformID EPM_BOARD_ID_IDP_V1P0{10};
const PlatformID EPM_BOARD_ID_DONGLE_V3P0{11};
const PlatformID EPM_BOARD_ID_MTP_V3P3{12};

PlatformIDs PlatformContainer::_platformIds;

_PlatformEntry::_PlatformEntry
(
	PlatformID platformID,
	DebugBoardType boardType,
	const QString& description,
	const QString& path,
	const QByteArray& usbDescriptor
) :
	_platformID(platformID),
	_boardtype(boardType),
	_description(description),
	_usbDescriptor(usbDescriptor),
	_path(path)
{
	_pinSets[0] = _pinSets[1] = _pinSets[2] = _pinSets[3] = NoOptions;
}

void PlatformContainer::initialize()
{
	if (_platformIds.isEmpty())
	{
		_platformIds.insert(MICRO_EPM_BOARD_ID_SPMV4, PlatformEntry(new _PlatformEntry(MICRO_EPM_BOARD_ID_SPMV4, ePSOC, "SPM V4")));
		_platformIds.insert(MICRO_EPM_BOARD_ID_EPMV4, PlatformEntry(new _PlatformEntry(MICRO_EPM_BOARD_ID_EPMV4, ePSOC, "EPM V4")));
		_platformIds.insert(MICRO_EPM_BOARD_ID_ALPACA, PlatformEntry(new _PlatformEntry(MICRO_EPM_BOARD_ID_ALPACA, ePSOC, "Alpaca V1")));
		_platformIds.insert(MICRO_EPM_BOARD_ID_MICROEPM_TAC, PlatformEntry(new _PlatformEntry(MICRO_EPM_BOARD_ID_MICROEPM_TAC, ePSOC, "MicroEPM TAC")));
		_platformIds.insert(MICRO_EPM_BOARD_ID_ALPACA_V2, PlatformEntry(new _PlatformEntry(MICRO_EPM_BOARD_ID_ALPACA_V2, ePSOC, "Alpaca V2")));
		_platformIds.insert(MICRO_EPM_BOARD_ID_ALPACA_V3, PlatformEntry(new _PlatformEntry(MICRO_EPM_BOARD_ID_ALPACA_V3, ePSOC, "Alpaca V3.0")));
		_platformIds.insert(MICRO_EPM_BOARD_ID_ALPACA_V3P1, PlatformEntry(new _PlatformEntry(MICRO_EPM_BOARD_ID_ALPACA_V3P1, ePSOC, "Alpaca V3.1")));
		_platformIds.insert(EPM_BOARD_ID_MTP_V3P2, PlatformEntry(new _PlatformEntry(EPM_BOARD_ID_MTP_V3P2, ePSOC, "MTP V3.2")));
		_platformIds.insert(EPM_BOARD_ID_QRD_V1P0, PlatformEntry(new _PlatformEntry(EPM_BOARD_ID_QRD_V1P0, ePSOC, "QRD V1.0")));
		_platformIds.insert(EPM_BOARD_ID_IDP_V1P0, PlatformEntry(new _PlatformEntry(EPM_BOARD_ID_IDP_V1P0, ePSOC, "IDP V1.0")));
		_platformIds.insert(EPM_BOARD_ID_DONGLE_V3P0, PlatformEntry(new _PlatformEntry(EPM_BOARD_ID_DONGLE_V3P0, ePSOC, "Dongle V3.0")));
		_platformIds.insert(EPM_BOARD_ID_MTP_V3P3, PlatformEntry(new _PlatformEntry(EPM_BOARD_ID_MTP_V3P3, ePSOC, "MTP V3.3")));

		PlatformEntry ftdiEntry(new _PlatformEntry(ALPACA_LITE_ID, eFTDI, "ALPACA LITE (FTDI)"));
		ftdiEntry->_usbDescriptor = "ALPACA-LITE MTP DEBUG BOARD";
		ftdiEntry->_pinSets[0].setFlag(eC);
		ftdiEntry->_pinSets[0].setFlag(eD);

		_platformIds.insert(ALPACA_LITE_ID, ftdiEntry);

		PlatformEntry pic32cxEntry(new _PlatformEntry(ALPACA_PIC32CX_ID, ePIC32CXAuto, "Default Automotive (PIC32CX)"));

		_platformIds.insert(ALPACA_PIC32CX_ID, pic32cxEntry);

		initializeDynamic();
	}
}

PlatformIDList PlatformContainer::getEntries()
{
	PlatformIDList result;

	for (const auto& platformEntry: std::as_const(_platformIds))
	{
		result.push_back(platformEntry);
	}

	return result;
}

void PlatformContainer::initializeDynamic()
{
	USBDescriptors usbDescriptors;

	QString loadPath;

	loadPath = tacConfigRoot() + kUSBDescriptorFileName;

	AppCore::writeToApplicationLog("Descriptor Load Path:" + loadPath + "\n");

	if (usbDescriptors.load(loadPath) == true)
	{
		DescriptorList descriptorList = usbDescriptors.getDescriptors();

		for (const auto& descriptor: descriptorList)
		{
			if (_platformIds.find(descriptor._platformID) == _platformIds.end())
			{
				PlatformEntry platformEntry(new _PlatformEntry(descriptor._platformID, descriptor._debugBoardType, descriptor._description));

				platformEntry->_usbDescriptor = descriptor._usbDescriptor;
				platformEntry->_path = descriptor._configurationFilePath;
				platformEntry->_pinSets[0] = descriptor._pinSets[0];
				platformEntry->_pinSets[1] = descriptor._pinSets[1];
				platformEntry->_pinSets[2] = descriptor._pinSets[2];
				platformEntry->_pinSets[3] = descriptor._pinSets[3];

				_platformIds.insert(descriptor._platformID, platformEntry);
			}
		}
	}
	else
	{
		if (AppCore::getAppCore()->appLoggingActive())
			AppCore::writeToApplicationLog("[PlatformContainer::initializeDynamic]: " + usbDescriptors.getLastError());
	}

	if (AppCore::getAppCore()->appLoggingActive())
	{
		for (const auto& platformId: std::as_const(_platformIds))
		{
			AppCore::writeToApplicationLog("   " + QString::number(platformId->_platformID) + " " + platformId->_description + "\n");
		}
	}
}

void PlatformContainer::addEntry
(
	PlatformEntry platformEntry
)
{
	_platformIds[platformEntry->_platformID] = platformEntry;
}

QString PlatformContainer::toString(PlatformID platformID)
{
	QString result{QString("Unknown Board ID: %1").arg(platformID)};

	if (_platformIds.find(platformID) != _platformIds.end())
		result = (*_platformIds[platformID])._description;

	return result;
}

PlatformID PlatformContainer::fromUSBDescriptor
(
	const QByteArray& usbDescriptor
)
{
	PlatformID result{MICRO_EPM_BOARD_ID_UNKNOWN};
	QByteArray descriptor{usbDescriptor.toLower()};

	for (const auto& platformEntry: RangedContainer(_platformIds))
	{
		QByteArray candidate = platformEntry.second->_usbDescriptor.toLower();
		if (candidate == descriptor)
		{
			result = platformEntry.second->_platformID;

			break;
		}
	}

	return result;
}

DebugBoardType PlatformContainer::getDebugBoardType
(
	PlatformID platformID
)
{
	DebugBoardType boardType{eUnknownDebugBoard};

	if (_platformIds.find(platformID) != _platformIds.end())
		boardType = _platformIds[platformID]->_boardtype;

	return boardType;
}

PlatformIDList PlatformContainer::getDebugBoards()
{
	PlatformIDList result;

	for (const auto& platformID: std::as_const(_platformIds))
		result.push_back(platformID);

	return result;
}

PlatformIDList PlatformContainer::getDebugBoardsOfType
(
	DebugBoardType debugBoardType
)
{
	PlatformIDList result;

	for (const auto& platformID: std::as_const(_platformIds))
		if (platformID->_boardtype == debugBoardType)
			result.push_back(platformID);

	return result;
}
