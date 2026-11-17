// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "PSOCDevice.h"
#include "private/TACPSOCDriveThread.h"
#include "SerialPortInfo.h"

// QCommonConsole
#include "AppCore.h"
#include "PSOCPlatformConfiguration.h"
#include "StringUtilities.h"

#include <algorithm>
#include <chrono>
#include <string>
#include <thread>

PSOCDevice::~PSOCDevice()
{
}

uint32_t PSOCDevice::updateAlpacaDevices()
{
	for (auto& alpacaDevice: _AlpacaDevice::_alpacaDevices)
	{
		alpacaDevice->setActive(false);
	}

	SerialPortInfos availablePorts = SerialPortInfo::availablePorts();
	for (const auto& serialInfo : availablePorts)
	{
		// Filter by PSoC VID/PID: (0x05C6, 0x9302) or (0x16C0, 0x0483)
		if (serialInfo.hasVendorIdentifier() && serialInfo.hasProductIdentifier())
		{
			bool match = (serialInfo.vendorIdentifier() == 0x05C6 && serialInfo.productIdentifier() == 0x9302) ||
			             (serialInfo.vendorIdentifier() == 0x16C0 && serialInfo.productIdentifier() == 0x0483);
			if (!match)
				continue;
		}

		std::string portName = serialInfo.portName();
		HashType hash = ::arrayHash(portName);

		AlpacaDevice alpacaDevice = _AlpacaDevice::findAlpacaDevice(hash);
		if (alpacaDevice == nullptr)
		{
			PSOCDevice* psocDevice = new PSOCDevice;

			psocDevice->_portName = portName;
			psocDevice->_hash = hash;

			psocDevice->_active = true;
			psocDevice->_boardType = ePSOC;
			psocDevice->_platformID = MICRO_EPM_BOARD_ID_UNKNOWN;
			psocDevice->_description = serialInfo.description();
			psocDevice->_serialNumber = serialInfo.serialNumber();

			_AlpacaDevice::_alpacaDevices.push_back(std::shared_ptr<PSOCDevice>(psocDevice));

			AppCore::writeToApplicationLog("Found a PSOC device with port name: '" + portName + "'\n");
		}
		else
		{
			alpacaDevice->setActive(true);
		}
	}

	return static_cast<uint32_t>(_AlpacaDevice::_alpacaDevices.size());
}

bool PSOCDevice::open()
{
	bool result{false};

	if (_driveThread == nullptr)
	{
		_driveThread = new TACPSOCDriveThread(_hash);
		_driveThread->start();

		std::this_thread::sleep_for(std::chrono::milliseconds(100)); // force DriveTrain thread to start faster

		for (int i{0}; i < 60; i++)
		{
			result = _driveThread->isRunning();
			if (result == true)
				break;
			if (_driveThread->hasFinished())
				break;
			std::this_thread::sleep_for(std::chrono::milliseconds(300));
		}

		result = false;
		for (int i{0}; i < 10; i++)
		{
			PlatformID platformID = _driveThread->platformID();
			if (platformID == MICRO_EPM_BOARD_ID_UNKNOWN)
			{
				if (_driveThread->oldFirmware() == false)
					std::this_thread::sleep_for(std::chrono::milliseconds(100));
				else
				{
					_lastError = "Firmware version is too old";
					break;
				}
			}
			else
			{
				_platformID = platformID;
				if (platformID != MICRO_EPM_BOARD_ID_UNKNOWN)
				{
					TACPlatformEntry platformEntry = _PlatformConfiguration::getEntry(_platformID);
					_description = platformEntry._platformEntry->_description;
					_platformConfiguration = platformEntry.getConfiguration();
					_psocPlatformConfiguration = static_cast<_PSOCPlatformConfiguration*>(_platformConfiguration.get());

					result = true;
					break;
				}
				else
				{
					result = false;
				}
			}
		}

		if (!result && _driveThread->connected())
		{
			// The board responded to commands, but board ID is unprogrammed (0).
			// Use MICRO_EPM_BOARD_ID_UNKNOWN / generic PSoC configuration so the user can interact with the device.
			_platformID = MICRO_EPM_BOARD_ID_UNKNOWN;
			TACPlatformEntry platformEntry = _PlatformConfiguration::getEntry(_platformID);
			if (platformEntry._platformEntry != nullptr)
			{
				_description = platformEntry._platformEntry->_description;
				_platformConfiguration = platformEntry.getConfiguration();
				_psocPlatformConfiguration = static_cast<_PSOCPlatformConfiguration*>(_platformConfiguration.get());
				result = true;
			}
		}

		if (result == false)
		{
			std::string threadErr = _driveThread->lastErrorMessage();
			if (!threadErr.empty())
				_lastError = threadErr;
			else if (_lastError.empty())
				_lastError = "Failed to communicate with device on port " + portName();
		}
	}

	if (result == true)
	{
		buildMapping();

		_driveThread->onPinStateChanged = [this](int pin, int state) {
			on_pinStateChanged(static_cast<uint64_t>(pin), state != 0);
		};
		_driveThread->onDeviceDisconnected = [this]() {
			onDeviceDisconnect();
		};
		_driveThread->onProgress = [this](int val) {
			if (onProgress) onProgress(val);
		};
		_driveThread->onError = [this](const std::string& err) {
			_lastError = err;
			if (onError) onError(err);
		};
		_driveThread->onDeviceStatusChange = [this](const std::string& status) {
			if (onDeviceStatusChange) onDeviceStatusChange(status);
		};

		Pins initialPins;
		Pins pins = _platformConfiguration->getPins();
		for (const auto& pin: pins)
		{
			if (pin._initializationPriority > 0)
			{
				initialPins.push_back(pin);
			}
		}

		// Once priority pins are set, append all initial pins without priority
		for (const auto& pin: pins)
		{
			if (pin._initialValue == true)
				initialPins.push_back(pin);
		}

		if (initialPins.size() > 0)
		{
			auto sortLambda = [] (PinEntry& p1, PinEntry& p2) -> bool
			{
				return p1._initializationPriority < p2._initializationPriority;
			};

			std::sort(initialPins.begin(), initialPins.end(), sortLambda);

			for (const auto& initializationPin: initialPins)
			{
				_driveThread->setPinState(initializationPin._pin, true);
				if (AppCore::getAppCore()->appLoggingActive())
				{
					std::string message = "PSOCDevice::open() Initialize Pin: " + std::to_string(initializationPin._pin) + "\n";
					AppCore::writeToApplicationLog(message);
				}
			}
		}
	}

	_chipVersion = _driveThread->chipVersion();

	return result;
}

void PSOCDevice::buildMapping()
{
	if (_commands.empty() == true)
	{
		Pins pins = _platformConfiguration->getPins();
		for (const auto& pin: std::as_const(pins))
		{
			TACCommand tacCommand;

			tacCommand._pin = pin._pin;
			tacCommand._command = pin._pinCommand;
			tacCommand._helpText = pin._pinTooltip;
			tacCommand._tabName = pin._tabName;
			tacCommand._groupName = CommandGroup::toString(pin._commandGroup);

			tacCommand._cellLocation = fromPoint(pin._cellLocation);

			_commands[tacCommand._command] = tacCommand;
			_commandList.push_back(tacCommand);
		}
	}

	buildQuickSettings();
}
