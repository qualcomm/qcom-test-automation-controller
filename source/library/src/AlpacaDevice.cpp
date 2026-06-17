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

// Authors: Michael Simpson, Biswajit Roy

#include <qtac/AlpacaDevice.h>
#include <qtac/TACDriveThread.h>

#include <algorithm>
#include <mutex>

// -----------------------------------------------------------------------
// Static members
// -----------------------------------------------------------------------

std::mutex     _AlpacaDevice::_mutex;
AlpacaDevices  _AlpacaDevice::_alpacaDevices;

// -----------------------------------------------------------------------

_AlpacaDevice::~_AlpacaDevice()
{
	if (isOpen())
		close();
}

void _AlpacaDevice::getAlpacaDevices(AlpacaDevices& alpacaDevices, DebugBoardType debugBoardTypeFilter)
{
	alpacaDevices.clear();
	std::lock_guard<std::mutex> lock(_mutex);

	for (auto& alpacaDevice : _alpacaDevices)
	{
		if (!alpacaDevice->active())
			continue;
		if (debugBoardTypeFilter == eUnknownDebugBoard)
			alpacaDevices.append(alpacaDevice);
		else if (alpacaDevice->debugBoardType() == debugBoardTypeFilter)
			alpacaDevices.append(alpacaDevice);
	}
}

uint32_t _AlpacaDevice::updateAlpacaDevices()
{
	_alpacaDevices.clear();
	std::lock_guard<std::mutex> lock(_mutex);
	// Concrete device classes (FTDIDevice) call their own updateAlpacaDevices()
	// and push into _alpacaDevices. See FTDIDevice::updateAlpacaDevices().
	return static_cast<uint32_t>(_alpacaDevices.size());
}

AlpacaDevice _AlpacaDevice::findAlpacaDevice(HashType hash)
{
	for (const auto& dev : _alpacaDevices)
		if (dev->_hash == hash)
			return dev;
	return AlpacaDevice(nullptr);
}

AlpacaDevice _AlpacaDevice::findAlpacaDevice(const qtac::ByteArray& portName)
{
	qtac::ByteArray searchTerm = portName.toLower();
	for (const auto& dev : _alpacaDevices)
	{
		if (dev->_portName.toLower().contains(searchTerm))
			return dev;
		if (dev->_serialNumber.toLower().contains(searchTerm))
			return dev;
	}
	return AlpacaDevice(nullptr);
}

AlpacaDevice _AlpacaDevice::findAlpacaDeviceBySerialNumber(const qtac::ByteArray& serialNumber, bool usePartial)
{
	qtac::ByteArray testSerial = serialNumber.toLower();
	for (const auto& dev : _alpacaDevices)
	{
		qtac::ByteArray devSerial = dev->_serialNumber.toLower();
		if (usePartial ? testSerial.contains(devSerial) : testSerial == devSerial)
			return dev;
	}
	return AlpacaDevice(nullptr);
}

AlpacaDevice _AlpacaDevice::findAlpacaDeviceByDescription(const qtac::ByteArray& description, bool usePartial)
{
	qtac::ByteArray testDesc = description.toLower();
	for (const auto& dev : _alpacaDevices)
	{
		qtac::ByteArray devDesc = dev->description().toLower();
		if (usePartial ? testDesc.contains(devDesc) : testDesc == devDesc)
			return dev;
	}
	return AlpacaDevice(nullptr);
}

AlpacaDevice _AlpacaDevice::findAlpacaDeviceByUSBDescriptor(const qtac::ByteArray& descriptor, bool usePartial)
{
	qtac::ByteArray testDescriptor = descriptor.toLower();
	for (const auto& dev : _alpacaDevices)
	{
		qtac::ByteArray devDescriptor = dev->_usbDescriptor.toLower();
		if (usePartial ? testDescriptor.contains(devDescriptor) : testDescriptor == devDescriptor)
			return dev;
	}
	return AlpacaDevice(nullptr);
}

qtac::ByteArray _AlpacaDevice::getLastError()
{
	qtac::ByteArray temp = _lastError;
	_lastError.clear();
	return temp;
}

bool _AlpacaDevice::isOpen()
{
	return _driveThread != nullptr || _serialDriveThread != nullptr;
}

void _AlpacaDevice::close()
{
	if (_driveThread != nullptr)
	{
		_driveThread->shutDown();
		delete _driveThread;
		_driveThread = nullptr;
	}
	if (_serialDriveThread != nullptr)
	{
		_serialDriveThread->shutDown();
		delete _serialDriveThread;
		_serialDriveThread = nullptr;
	}
}

uint32_t _AlpacaDevice::commandCount()
{
	commandList();
	return static_cast<uint32_t>(_commandList.size());
}

TACCommand _AlpacaDevice::commandEntry(uint32_t commandIndex)
{
	if (commandIndex < static_cast<uint32_t>(_commandList.size()))
		return _commandList.at(commandIndex);
	return TACCommand{};
}

TACCommands _AlpacaDevice::commandList()
{
	if (_commandList.isEmpty())
	{
		for (const auto& entry : _commands)
			_commandList.append(entry.second);

		std::stable_sort(_commandList.begin(), _commandList.end(),
			[](const TACCommand& a, const TACCommand& b) { return a._command < b._command; });
	}
	return _commandList;
}

bool _AlpacaDevice::getCommandState(const qtac::ByteArray& command)
{
	qtac::String key(command.toStdString());
	if (_commands.contains(key))
		return _commands.value(key)._currentState;
	return false;
}

bool _AlpacaDevice::sendCommand(const qtac::ByteArray& command, bool state)
{
	if (_driveThread == nullptr)
		return false;

	qtac::String key(command.toStdString());
	if (_commands.contains(key))
	{
		setPinState(_commands.value(key)._pin, state);
		_driveThread->clearWaitForCompletion();
		return true;
	}
	_driveThread->clearWaitForCompletion();
	return false;
}

bool _AlpacaDevice::isCommandQueueClear()
{
	if (_driveThread != nullptr && active())
		return _driveThread->waitForCompletionStatus();
	return false;
}

qtac::ByteArray _AlpacaDevice::getHelp()
{
	return _helpText;
}

void _AlpacaDevice::setPinState(PinID pin, bool state)
{
	if (_driveThread != nullptr && active())
		_driveThread->setPinState(pin, state);
}

void _AlpacaDevice::setWaitForCompletion()
{
	if (_driveThread != nullptr)
		_driveThread->setWaitForCompletion();
}

bool _AlpacaDevice::active()
{
	return _active;
}

HashType _AlpacaDevice::hash()
{
	return _hash;
}

qtac::ByteArray _AlpacaDevice::description()
{
	return _description;
}

qtac::ByteArray _AlpacaDevice::usbDescriptor()
{
	return _usbDescriptor;
}

qtac::ByteArray _AlpacaDevice::serialNumber()
{
	return _serialNumber;
}

PlatformID _AlpacaDevice::platformID()
{
	return _platformID;
}

qtac::ByteArray _AlpacaDevice::macAddress()
{
	if (_driveThread)
		return _driveThread->macAddress();
	return qtac::ByteArray();
}

DebugBoardType _AlpacaDevice::debugBoardType()
{
	if (_boardType == eUnknownDebugBoard && _driveThread)
		_boardType = _driveThread->debugBoardType();
	return _boardType;
}

qtac::String _AlpacaDevice::debugBoardTypeString()
{
	if (_driveThread) return _driveThread->debugBoardTypeString();
	return qtac::String();
}

qtac::String _AlpacaDevice::hardwareVersionString()
{
	if (_driveThread) return _driveThread->hardwareVersionString();
	return qtac::String();
}

qtac::String _AlpacaDevice::firmwareVersion()
{
	if (_driveThread) return _driveThread->firmwareVersion();
	return qtac::String();
}

unsigned int _AlpacaDevice::majorVersion()
{
	if (_driveThread) return _driveThread->majorVersion();
	return 0;
}

qtac::String _AlpacaDevice::chipVersion()
{
	switch (_chipVersion)
	{
	case 3:     return "LP038";
	case 4:     return "LP030";
	case 10000: return "FTDI";
	default:    return "None";
	}
}

unsigned int _AlpacaDevice::minorVersion()
{
	if (_driveThread) return _driveThread->minorVersion();
	return 0;
}

unsigned int _AlpacaDevice::revisionVersion()
{
	if (_driveThread) return _driveThread->revisionVersion();
	return 0;
}

void _AlpacaDevice::externalPowerControl(bool state)
{
	qtac::String key("extpower");
	if (_commands.contains(key))
		setPinState(_commands.value(key)._pin, state);
}

qtac::ByteArray _AlpacaDevice::portName() const
{
	return _portName;
}

void _AlpacaDevice::setPortName(const qtac::ByteArray& portName)
{
	_portName = portName;
	if (_driveThread)
		_driveThread->setPortName(portName);
}

qtac::ByteArray _AlpacaDevice::name() const
{
	if (_driveThread) return _driveThread->name();
	return _serialNumber;
}

void _AlpacaDevice::setName(const qtac::ByteArray& newName)
{
	if (_driveThread)
		_driveThread->setName(newName);
}

qtac::String _AlpacaDevice::descriptionStr() const
{
	if (_driveThread) return _driveThread->description();
	return qtac::String();
}

void _AlpacaDevice::setDescription(const qtac::String& description)
{
	if (_driveThread)
		_driveThread->setDescription(description);
}

qtac::String _AlpacaDevice::serialNumberStr() const
{
	if (_driveThread) return _driveThread->serialNumber();
	return qtac::String();
}

void _AlpacaDevice::setSerialNumber(const qtac::String& serialNumber)
{
	_serialNumber = serialNumber.toLatin1();
	if (_driveThread)
		_driveThread->setSerialNumber(serialNumber);
}

qtac::String _AlpacaDevice::uuid()
{
	if (_driveThread) return _driveThread->uuid();
	return qtac::String();
}

qtac::Size _AlpacaDevice::windowDimension()
{
	return qtac::Size(560, 750);
}

int _AlpacaDevice::getResetCount()
{
	if (_driveThread != nullptr) return _driveThread->getResetCount();
	return 0;
}

void _AlpacaDevice::clearResetCount()
{
	if (_driveThread != nullptr) _driveThread->clearResetCount();
}

void _AlpacaDevice::i2CReadRegister(uint32_t addr, uint32_t reg)
{
	if (_driveThread != nullptr) _driveThread->i2CReadRegister(addr, reg);
}

void _AlpacaDevice::i2CWriteRegister(uint32_t addr, uint32_t reg, uint32_t data)
{
	if (_driveThread != nullptr) _driveThread->i2CWriteRegister(addr, reg, data);
}

void _AlpacaDevice::on_pinStateChanged(uint64_t pin, bool state)
{
	for (auto& kv : _commands)
	{
		if (kv.second._pin == pin)
		{
			kv.second._currentState = state;
			break;
		}
	}
	if (onPinStateChanged)
		onPinStateChanged(pin, state);
}

void _AlpacaDevice::onDeviceDisconnect()
{
	close();
}
