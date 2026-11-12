// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "STM32Device.h"

#include "private/TACSTM32DriveThread.h"

// QCommonConsole
#include "AlpacaSettings.h"
#include "AppCore.h"
#include "STM32PlatformConfiguration.h"

// Qt
#include <QThread>

// hidapi
#include <hidapi.h>

#if defined(_WIN32)
#include <windows.h>
#endif
#include <cstdlib>
#include <cwchar>
#include <cstdint>

const uint16_t kSTM32VendorID{0x2341};
const uint16_t kSTM32ProductID{0xB001};
const uint16_t kSTM32UsagePage{0xFF60};
const uint16_t kSTM32Usage{0x0061};

static bool hidapiInitialized{false};


bool isSTMInterface(const hid_device_info& info)
{
	return info.usage_page == kSTM32UsagePage && info.usage == kSTM32Usage;
}

STM32Device::~STM32Device()
{

}

bool STM32Device::programDevice(AlpacaDevice alpacaDevice, PlatformID platformID, std::string& errorMessage)
{
	Q_UNUSED(alpacaDevice)
	Q_UNUSED(platformID)

	errorMessage = "STM32 device does not support firmware programming";

	return false;
}

STM32HIDDeviceInfoList STM32Device::enumerateHIDDevices()
{
	if (hidapiInitialized == false)
	{
		hid_init();
		hidapiInitialized = true;
	}

	STM32HIDDeviceInfoList result;

	struct hid_device_info* devices = hid_enumerate(kSTM32VendorID, kSTM32ProductID);

	for (struct hid_device_info* current = devices; current != nullptr; current = current->next)
	{
		if (isSTMInterface(*current) == false)
			continue;

		STM32HIDDeviceInfo deviceInfo;

		std::string devicePath{current->path ? current->path : ""};
		std::string serialNumber;
		if (current->serial_number)
		{
#if defined(_WIN32)
			int len = WideCharToMultiByte(CP_UTF8, 0, current->serial_number, -1, nullptr, 0, nullptr, nullptr);
			if (len > 0)
			{
				serialNumber.resize(len);
				WideCharToMultiByte(CP_UTF8, 0, current->serial_number, -1, &serialNumber[0], len, nullptr, nullptr);
				if (!serialNumber.empty() && serialNumber.back() == '\0')
					serialNumber.pop_back();
			}
#else
			size_t len = std::wcstombs(nullptr, current->serial_number, 0);
			if (len != static_cast<size_t>(-1))
			{
				serialNumber.resize(len);
				std::wcstombs(&serialNumber[0], current->serial_number, len);
			}
#endif
		}

		if (!devicePath.empty() && !serialNumber.empty())
		{
			deviceInfo._devicePath = devicePath;
			deviceInfo._serialNumber = serialNumber;

			const std::string& hashSource = deviceInfo._serialNumber.empty() == false ? deviceInfo._serialNumber : deviceInfo._devicePath;
			deviceInfo._hash = ::arrayHash(hashSource);

			result.push_back(deviceInfo);
		}
	}

	hid_free_enumeration(devices);

	return result;
}

std::string STM32Device::vtpPortName
(
	const std::string& serialNumber
)
{
	std::string result;

	if (serialNumber.empty() == false)
	{
		AlpacaSettings settings("Alpaca");

		settings.beginGroup("VTPs");
		unsigned int nextPort = settings.value("NextPortNumber", 1).toUInt();

		std::string vtpName = settings.value(serialNumber, std::string()).toString();
		if (vtpName.empty())
		{
			vtpName = "VTP" + std::to_string(nextPort);
			settings.setValue(serialNumber, vtpName);

			nextPort++;

			settings.setValue("NextPortNumber", nextPort);
		}

		settings.endGroup();

		result = vtpName;
	}

	return result;
}

uint32_t STM32Device::updateAlpacaDevices()
{
	STM32HIDDeviceInfoList deviceInfoList = enumerateHIDDevices();

	for (const auto& deviceInfo: std::as_const(deviceInfoList))
	{
		AlpacaDevice alpacaDevice = _AlpacaDevice::findAlpacaDevice(deviceInfo._hash);
		if (!alpacaDevice)
		{
			STM32Device* stm32Device = new STM32Device;

			stm32Device->_active = true;
			stm32Device->_portName = vtpPortName(deviceInfo._serialNumber);
			stm32Device->_hash = deviceInfo._hash;
			stm32Device->_chipVersion = 2;
			stm32Device->_serialNumber = deviceInfo._serialNumber;
			stm32Device->_platformID = ALPACA_STM32_ID;
			stm32Device->_devicePath = deviceInfo._devicePath;

			TACPlatformEntry platformEntry = _PlatformConfiguration::getEntry(stm32Device->_platformID);
			stm32Device->_description = platformEntry._platformEntry->_description;
			stm32Device->_boardType = platformEntry._platformEntry->_boardtype;

			stm32Device->_platformConfiguration = platformEntry.getConfiguration();

			alpacaDevice = AlpacaDevice(stm32Device);
			_AlpacaDevice::_alpacaDevices.push_back(alpacaDevice);

			AppCore::writeToApplicationLog("Found a STM32 device with path: '" + stm32Device->portName() + "' and USB descriptor string: '" + stm32Device->usbDescriptor() + "'\n");
		}
		else
		{
			alpacaDevice->setActive();
		}
	}

	return _AlpacaDevice::_alpacaDevices.count();
}

bool STM32Device::open()
{
	const int maxInterations{50};

	bool result{false};

	if (_platformConfiguration != nullptr)
	{
		_stm32PlatformConfiguration = static_cast<_STM32PlatformConfiguration*>(_platformConfiguration.get());

		buildMapping();

		if (_driveThread == nullptr)
		{
			TACSTM32DriveThread* driveThread = new TACSTM32DriveThread(_hash);

			_driveThread = driveThread;
			_driveThread->start();

			int count = 0;
			while (count < maxInterations)
			{
				QThread::msleep(20);

				result = _driveThread->weAreRunning();
				if (result == true)
					count = maxInterations;

				count++;
			}

			if (result == true)
				connect(driveThread, &TACSTM32DriveThread::pinStateChanged, this, &STM32Device::on_pinStateChanged, Qt::DirectConnection);
		}
	}

	return result;
}

void STM32Device::buildMapping()
{
	Q_ASSERT(_stm32PlatformConfiguration != nullptr);

	buildCommandList();
	buildQuickSettings();
}

void STM32Device::buildCommandList()
{
	if (_commands.empty() == true)
	{
		if (_stm32PlatformConfiguration != nullptr)
		{
			STM32PinList stm32PinList = _stm32PlatformConfiguration->getActivePins();

			for (const auto& stm32Pin: std::as_const(stm32PinList))
			{
				TACCommand tacCommand;

				tacCommand._pin = stm32Pin._pin;
				tacCommand._command = stm32Pin._pinCommand;
				tacCommand._helpText = stm32Pin._pinTooltip;
				tacCommand._isInverted = stm32Pin._inverted;
				tacCommand._tabName = stm32Pin._tabName;
				tacCommand._groupName = CommandGroup::toString(stm32Pin._commandGroup);
				tacCommand._cellLocation = stm32Pin._cellLocation;

				_commands[tacCommand._command] = tacCommand;
			}
		}
	}
}

bool STM32Device::openTransport()
{
	bool result{false};

	if (_devicePath.empty() == false)
	{
		if (_hidHandle == nullptr)
		{
			_hidHandle = hid_open_path(_devicePath.c_str());

			if (_hidHandle != nullptr)
			{
				AppCore::writeToApplicationLogLine("Opened Arduino BugHopper V2 device " + _devicePath + "\n");

				result = true;
			}
			else
			{
				AppCore::writeToApplicationLogLine("STM32Device::openTransport() hid_open_path() failed");
			}
		}
		else
		{
			result = true;
		}
	}
	else
	{
		AppCore::writeToApplicationLogLine("STM32Device::openTransport() _devicePath is empty");
	}

	return result;
}

bool STM32Device::write
(
	const std::string& report
)
{
	bool result{false};

	if (_hidHandle != nullptr)
	{
		result = hid_write(static_cast<hid_device*>(_hidHandle), reinterpret_cast<const unsigned char*>(report.data()), report.size()) >= 0;
	}

	return result;
}

void STM32Device::close()
{
	if (_hidHandle != nullptr)
	{
		hid_close(static_cast<hid_device*>(_hidHandle));
		_hidHandle = nullptr;
	}
}
