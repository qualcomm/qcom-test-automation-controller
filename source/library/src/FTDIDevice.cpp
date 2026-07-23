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
#include <qtac/PlatformID.h>
#include <qtac/TcnfLoader.h>

// FTDI D2XX
#include "ftd2xx.h"

#include <cstring>
#include <thread>
#include <chrono>

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

	// The drive thread must be injected by the caller before calling open().
	if (_driveThread == nullptr)
	{
		_lastError = "No drive thread set";
		return false;
	}

	// Load platform configuration before starting the drive thread so we can
	// configure the correct bus pinset (D2XX-only, skipping VCP buses).
	if (_ftdiPlatformConfiguration == nullptr && _platformID != MICRO_EPM_BOARD_ID_UNKNOWN)
	{
		PlatformIDList entries = PlatformContainer::getEntries();
		for (auto& entry : entries)
		{
			if (entry && entry->_platformID == _platformID)
			{
				if (!entry->_path.isEmpty())
				{
					auto* cfg = new _FTDIPlatformConfiguration(0);
					if (TcnfLoader::loadFTDI(entry->_path.toStdString(), cfg))
						_ftdiPlatformConfiguration = cfg;
					else
						delete cfg;
				}
				else if (_platformID == ALPACA_LITE_ID)
				{
					// Generic ALPACA-LITE board: no tcnf registered, use hardcoded
					// default pin configuration (same as Qt's initialize(chipCount=1)).
					_ftdiPlatformConfiguration = new _FTDIPlatformConfiguration(1);
				}
				break;
			}
		}
	}

	// Only open D2XX buses (skip VCP buses A/B for devices like platform 13).
	// Mirrors legacy FTDIDevice::open() which calls setPinSets(getPinSet(0)).
	// Must be set before start() so FTDIChipset::open() uses the correct mask.
	if (_ftdiPlatformConfiguration != nullptr)
		_driveThread->setPinSets(_ftdiPlatformConfiguration->getPinSet(0));

	_driveThread->start();

	for (int count = 0; count < maxIterations; ++count)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
		result = _driveThread->weAreRunning();
		if (result) break;
	}

	if (!result)
	{
		_lastError = "Device did not become ready after open";
		return false;
	}

	if (_ftdiPlatformConfiguration != nullptr)
		buildMapping();

	if (result)
	{
		// Wire up pin-state signal
		_driveThread->onPinStateChanged.connect([this](uint64_t pin, bool state) {
			this->on_pinStateChanged(pin, state);
		});

		// Initialize pins with their initial values, sorted by priority.
		// Use getAllPins() (not getPins()/getActivePins()) so that disabled
		// pins with initial_value=true — e.g. TC_READY_N — are also written.
		// TC_READY_N must be driven HIGH before any script pin writes take effect.
		FTDIPinList initialFtdiPins;
		if (_ftdiPlatformConfiguration != nullptr)
		{
			FTDIPinList allFtdiPins = _ftdiPlatformConfiguration->getAllPins();
			for (const auto& pin : allFtdiPins)
				if (pin._initialValue)
					initialFtdiPins.append(pin);
		}

		if (!initialFtdiPins.isEmpty())
		{
			std::sort(initialFtdiPins.begin(), initialFtdiPins.end(),
				[](const FTDIPinData& a, const FTDIPinData& b) {
					return a._initializationPriority < b._initializationPriority;
				});

			for (const auto& initPin : initialFtdiPins)
				_driveThread->setPinState(initPin._setPin, initPin._initialValue);
		}
	}

	return result;
}

void FTDIDevice::buildMapping()
{
	if (_ftdiPlatformConfiguration == nullptr)
		return;
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

Pins FTDIDevice::getPins()
{
	if (_ftdiPlatformConfiguration == nullptr)
		return {};
	return _ftdiPlatformConfiguration->getPins();
}

qtac::StringList FTDIDevice::getTabs() const
{
	if (_ftdiPlatformConfiguration == nullptr)
		return {};
	return _ftdiPlatformConfiguration->getTabs();
}

void FTDIDevice::quickCommand(const qtac::ByteArray& command)
{
	if (_ftdiPlatformConfiguration == nullptr || _driveThread == nullptr)
		return;

	const qtac::AlpacaScript& script = _ftdiPlatformConfiguration->getScript();
	if (!script.hasCommand(command))
		return;

	qtac::CommandEntries entries = script.getCommandEntries(command);
	entries = qtac::AlpacaScript::replaceTokens(
	    _ftdiPlatformConfiguration->getVariables(), entries);

	_driveThread->sendCommandSequence(entries);
}

void FTDIDevice::setVariableValue(const qtac::String& name, const qtac::Variant& value)
{
	if (_ftdiPlatformConfiguration != nullptr)
		_ftdiPlatformConfiguration->setVariableValue(name, value);
}

const qtac::ButtonEntries& FTDIDevice::getButtons() const
{
	static qtac::ButtonEntries empty;
	if (_ftdiPlatformConfiguration == nullptr) return empty;
	return _ftdiPlatformConfiguration->getButtons();
}

const qtac::VariableEntries& FTDIDevice::getVariables() const
{
	static qtac::VariableEntries empty;
	if (_ftdiPlatformConfiguration == nullptr) return empty;
	return _ftdiPlatformConfiguration->getVariables();
}

qtac::String FTDIDevice::configModificationDate() const
{
	if (_ftdiPlatformConfiguration == nullptr) return {};
	return _ftdiPlatformConfiguration->modificationDate();
}

int FTDIDevice::configFileVersion() const
{
	if (_ftdiPlatformConfiguration == nullptr) return 0;
	return _ftdiPlatformConfiguration->fileVersion();
}
