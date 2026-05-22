// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted (subject to the limitations in the
// disclaimer below) provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//         notice, this list of conditions and the following disclaimer.
//
//     * Redistributions in binary form must reproduce the above
//         copyright notice, this list of conditions and the following
//         disclaimer in the documentation and/or other materials provided
//         with the distribution.
//
//     * Neither the name of Qualcomm Technologies, Inc. nor the names of its
//         contributors may be used to endorse or promote products derived
//         from this software without specific prior written permission.
//
// NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
// GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
// HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
// IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
// IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Author: Michael Simpson

#include <qtac/FTDIDevice.h>
#include <qtac/FTDIChipset.h>
#include <qtac/StringUtilities.h>

// FTDI D2XX
#include "ftd2xx.h"

#include <cstring>
#include <thread>
#include <chrono>
#include <cassert>

// -----------------------------------------------------------------------
// FTDI library lifetime management (static init/finalise for static D2XX)
// -----------------------------------------------------------------------

namespace {

struct FTDIInitializer
{
	FTDIInitializer()
	{
#if defined(FTD2XX_STATIC)
		FT_Initialise();
#endif
	}
	~FTDIInitializer()
	{
#if defined(FTD2XX_STATIC)
		FT_Finalise();
#endif
	}
} gFTDIInitializer;

} // anonymous namespace

// -----------------------------------------------------------------------

bool FTDIDevice::programDevice(AlpacaDevice alpacaDevice,
                                PlatformID platformID,
                                qtac::ByteArray& errorMessage)
{
	bool result{false};

	// Retrieve the platform entry
	PlatformIDList entries = PlatformContainer::getEntries();
	_PlatformEntry* matchedEntry = nullptr;
	for (auto& entry : entries)
	{
		if (entry && entry->_platformID == platformID)
		{
			matchedEntry = entry.get();
			break;
		}
	}

	if (matchedEntry == nullptr)
		return false;

	FT_STATUS ftStatus;
	FT_HANDLE ftHandle{nullptr};

	qtac::ByteArray serialNumber = alpacaDevice->serialNumber() + "A";
	qtac::ByteArray usbDescriptor = matchedEntry->_usbDescriptor;

	if (usbDescriptor.isEmpty())
		usbDescriptor = "ALPACA-LITE MTP DEBUG BOARD";

	ftStatus = FT_OpenEx(serialNumber.data(), FT_OPEN_BY_SERIAL_NUMBER, &ftHandle);
	if (ftStatus == FT_OK)
	{
		FT_PROGRAM_DATA ftData;
		::memset(&ftData, 0, sizeof(FT_PROGRAM_DATA));

		char manufacturerBuf[32];
		char manufacturerIdBuf[16];
		char descriptionBuf[64];
		char serialNumberBuf[16];

		ftData.Signature1   = 0x00000000;
		ftData.Signature2   = 0xffffffff;
		ftData.Version      = 0x00000008;
		ftData.Manufacturer = manufacturerBuf;
		ftData.ManufacturerId = manufacturerIdBuf;
		ftData.Description  = descriptionBuf;
		ftData.SerialNumber = serialNumberBuf;

		ftStatus = FT_EE_Read(ftHandle, &ftData);
		if (ftStatus == FT_OK)
		{
			size_t copyLen = static_cast<size_t>(usbDescriptor.size());
			if (copyLen >= sizeof(descriptionBuf))
				copyLen = sizeof(descriptionBuf) - 1;
			::memcpy(descriptionBuf, usbDescriptor.data(), copyLen);
			descriptionBuf[copyLen] = '\0';

			ftData.AIsVCP8 = testFlag(matchedEntry->_pinSets[0], eA) ? 0 : 1;
			ftData.BIsVCP8 = testFlag(matchedEntry->_pinSets[0], eB) ? 0 : 1;
			ftData.CIsVCP8 = testFlag(matchedEntry->_pinSets[0], eC) ? 0 : 1;
			ftData.DIsVCP8 = testFlag(matchedEntry->_pinSets[0], eD) ? 0 : 1;

			ftStatus = FT_EE_Program(ftHandle, &ftData);
			if (ftStatus == FT_OK)
			{
				result = true;
				FT_CyclePort(ftHandle);
			}
			else
			{
				errorMessage = ("FT_EE_Program failed Error Code: "
				               + _FTDIChipset::ftidStatusToString(ftStatus) + "\n").toLatin1();
			}
		}
		else
		{
			errorMessage = ("FT_EE_Read failed Error Code: "
			               + _FTDIChipset::ftidStatusToString(ftStatus) + "\n").toLatin1();
		}

		FT_Close(ftHandle);
	}
	else
	{
		errorMessage = ("FT_OpenEx failed Error Code: "
		               + _FTDIChipset::ftidStatusToString(ftStatus) + "\n").toLatin1();
	}

	return result;
}

uint32_t FTDIDevice::updateAlpacaDevices()
{
	uint32_t deviceCount = _FTDIChipset::getDeviceCount();

	for (uint32_t deviceIndex = 0; deviceIndex < deviceCount; ++deviceIndex)
	{
		FTDIChipset ftdiChipset = _FTDIChipset::getDevice(deviceIndex);

		HashType hash = ftdiChipset->hash();

		AlpacaDevice alpacaDevice = _AlpacaDevice::findAlpacaDevice(hash);
		if (alpacaDevice == nullptr)
		{
			FTDIDevice* ftdiDevice = new FTDIDevice;

			ftdiDevice->_active          = true;
			ftdiDevice->_boardType       = eFTDI;
			ftdiDevice->_portName        = ftdiChipset->portName();
			ftdiDevice->_hash            = hash;
			ftdiDevice->_chipVersion     = 10000;
			ftdiDevice->_usbDescriptor   = ftdiChipset->usbDescriptor();
			ftdiDevice->_serialNumber    = ftdiChipset->serialNumber();
			ftdiDevice->_platformID      = ftdiChipset->platformID();

			// Find description from platform registry
			PlatformIDList entries = PlatformContainer::getEntries();
			for (auto& entry : entries)
			{
				if (entry && entry->_platformID == ftdiDevice->_platformID)
				{
					ftdiDevice->_description = entry->_description.toLatin1();
					break;
				}
			}

			alpacaDevice = AlpacaDevice(ftdiDevice);
			_AlpacaDevice::_alpacaDevices.append(alpacaDevice);
		}
		else
		{
			alpacaDevice->setActive();
		}
	}

	return static_cast<uint32_t>(_AlpacaDevice::_alpacaDevices.size());
}

bool FTDIDevice::open()
{
	const int maxIterations{50};
	bool result{false};

	// We need a concrete drive thread; since we are in source/library (Qt-free),
	// the caller is responsible for injecting a concrete IFTDIDriveThread before
	// calling open(), OR subclasses override open() to create the thread.
	// For the FTDI path the concrete thread (TACLiteDriveThread) lives in
	// qcommon-console and is injected by setting _driveThread before calling
	// this method.

	if (_driveThread == nullptr)
		return false;

	if (!_driveThread->start())
		return false;

	for (int count = 0; count < maxIterations; ++count)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
		result = _driveThread->weAreRunning();
		if (result) break;
	}

	if (result)
	{
		// Wire up pin-state callback
		_driveThread->onPinStateChanged = [this](uint64_t pin, bool state) {
			this->on_pinStateChanged(pin, state);
		};

		// Initialize pins with their initial values, sorted by priority
		Pins initialPins;
		if (_ftdiPlatformConfiguration != nullptr)
		{
			Pins allPins = _ftdiPlatformConfiguration->getPins();
			for (const auto& pin : allPins)
				if (pin._initialValue)
					initialPins.append(pin);
		}

		if (!initialPins.isEmpty())
		{
			std::sort(initialPins.begin(), initialPins.end(),
				[](const PinEntry& a, const PinEntry& b) {
					return a._initializationPriority < b._initializationPriority;
				});

			for (const auto& initPin : initialPins)
				_driveThread->setPinState(initPin._pin, initPin._initialValue);
		}
	}

	return result;
}

void FTDIDevice::buildMapping()
{
	assert(_ftdiPlatformConfiguration != nullptr);
	buildCommandList();
}

void FTDIDevice::buildCommandList()
{
	if (!_commands.isEmpty())
		return;

	if (_ftdiPlatformConfiguration == nullptr)
		return;

	FTDIPinList ftdiPinList = _ftdiPlatformConfiguration->getActivePins();

	for (const auto& ftdiPin : ftdiPinList)
	{
		TACCommand tacCommand;

		tacCommand._pin          = ftdiPin._setPin;
		tacCommand._command      = ftdiPin._pinCommand;
		tacCommand._helpText     = ftdiPin._pinTooltip;
		tacCommand._currentState = ftdiPin._initialValue;
		tacCommand._isInverted   = ftdiPin._inverted;
		tacCommand._tabName      = ftdiPin._tabName.toLatin1();
		tacCommand._groupName    = CommandGroup::toString(ftdiPin._commandGroup).toLatin1();

		qtac::ByteArray cellLocation =
			qtac::ByteArray::number(ftdiPin._cellLocation.x()) + ","
			+ qtac::ByteArray::number(ftdiPin._cellLocation.y());
		tacCommand._cellLocation = cellLocation;

		_commands[tacCommand._command] = tacCommand;
	}
}
