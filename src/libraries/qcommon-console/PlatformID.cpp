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
*/

#include "PlatformID.h"

// QCommon
#include "AppCore.h"
#include "ConsoleApplicationEnhancements.h"
#include "RangedContainer.h"
#include "USBDescriptors.h"


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
		PlatformEntry ftdiEntry(new _PlatformEntry(ALPACA_LITE_ID, eFTDI, "ALPACA LITE (FTDI)"));
		ftdiEntry->_usbDescriptor = "ALPACA-LITE MTP DEBUG BOARD";
		ftdiEntry->_pinSets[0].setFlag(eC);
		ftdiEntry->_pinSets[0].setFlag(eD);

		_platformIds.insert(ALPACA_LITE_ID, ftdiEntry);

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
	PlatformID result{ALPACA_LITE_ID};
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
