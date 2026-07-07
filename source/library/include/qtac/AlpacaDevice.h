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

#ifndef QTAC_ALPACADEVICE_H
#define QTAC_ALPACADEVICE_H

#include <qtac/ByteArray.h>
#include <qtac/String.h>
#include <qtac/List.h>
#include <qtac/Signal.h>
#include <qtac/StringUtilities.h>
#include <qtac/DebugBoardType.h>
#include <qtac/PlatformID.h>
#include <qtac/TACCommand.h>
#include <qtac/Size.h>
#include <qtac/PinEntry.h>
#include <qtac/AlpacaScript.h>

#include <cstdint>
#include <memory>
#include <mutex>

#include <qtac/TACDriveThread.h>

// -----------------------------------------------------------------------

const qtac::ByteArray kMACNotSupported{"Not supported"};

class _AlpacaDevice;

using AlpacaDevice  = std::shared_ptr<_AlpacaDevice>;
using AlpacaDevices = qtac::List<AlpacaDevice>;

class _AlpacaDevice
{
public:
	_AlpacaDevice() = default;
	virtual ~_AlpacaDevice();

	// --- Static device registry ---
	static void getAlpacaDevices(AlpacaDevices& alpacaDevices,
	                             DebugBoardType debugBoardTypeFilter = eUnknownDebugBoard);
	static uint32_t updateAlpacaDevices();

	static AlpacaDevice findAlpacaDevice(HashType hash);
	static AlpacaDevice findAlpacaDevice(const qtac::ByteArray& portName);
	static AlpacaDevice findAlpacaDeviceBySerialNumber(const qtac::ByteArray& serialNumber, bool usePartial = true);
	static AlpacaDevice findAlpacaDeviceByDescription(const qtac::ByteArray& description, bool usePartial = true);
	static AlpacaDevice findAlpacaDeviceByUSBDescriptor(const qtac::ByteArray& descriptor, bool usePartial = true);

	// --- Lifecycle ---
	qtac::ByteArray getLastError();
	virtual bool open() = 0;
	bool isOpen();
	void close();

	// --- Command interface ---
	virtual void buildMapping() = 0;
	uint32_t commandCount();
	TACCommand commandEntry(uint32_t commandIndex);
	TACCommands commandList();

	// --- Pin list (populated after open() + buildMapping()) ---
	virtual Pins getPins() { return {}; }

	// --- Quick Settings buttons / variables (FTDI devices) ---
	virtual const qtac::ButtonEntries&   getButtons()   const { static qtac::ButtonEntries   empty; return empty; }
	virtual const qtac::VariableEntries& getVariables() const { static qtac::VariableEntries empty; return empty; }

	bool getCommandState(const qtac::ByteArray& command);
	bool sendCommand(const qtac::ByteArray& command, bool state);
	bool isCommandQueueClear();
	qtac::ByteArray getHelp();

	virtual void setPinState(PinID pin, bool state);
	void setWaitForCompletion();
	bool active();
	HashType hash();

	// Execute a named Quick Settings script function.
	virtual void quickCommand(const qtac::ByteArray& /*command*/) {}

	// Update a variable value for script token substitution.
	virtual void setVariableValue(const qtac::String& /*name*/,
	                              const qtac::Variant& /*value*/) {}

	// --- Port / name ---
	qtac::ByteArray portName() const;
	void setPortName(const qtac::ByteArray& portName);
	qtac::ByteArray name() const;
	void setName(const qtac::ByteArray& newName);

	// --- Properties ---
	DebugBoardType debugBoardType();
	qtac::String debugBoardTypeString();
	qtac::String hardwareVersionString();
	qtac::String firmwareVersion();
	unsigned int majorVersion();
	qtac::String chipVersion();
	unsigned int minorVersion();
	unsigned int revisionVersion();

	qtac::ByteArray description();
	qtac::ByteArray usbDescriptor();
	qtac::ByteArray serialNumber();
	PlatformID platformID();
	qtac::ByteArray macAddress();

	void externalPowerControl(bool state);

	qtac::String descriptionStr() const;
	void setDescription(const qtac::String& description);
	qtac::String serialNumberStr() const;
	void setSerialNumber(const qtac::String& serialNumber);
	qtac::String uuid();

	qtac::Size windowDimension();

	int getResetCount();
	void clearResetCount();

	void i2CReadRegister(uint32_t addr, uint32_t reg);
	void i2CWriteRegister(uint32_t addr, uint32_t reg, uint32_t data);

	void setActive(bool active = true) { _active = active; }

	// Inject the concrete drive thread before calling open().
	// The caller retains ownership; the device does NOT delete it.
	void setDriveThread(qtac::TACDriveThread* dt) { _driveThread = dt; }

	// --- Signals (replaces Qt signals) ---
	qtac::Signal<uint64_t, bool>               onPinStateChanged;
	qtac::Signal<uint8_t, int>                 onProgress;
	qtac::Signal<const qtac::ByteArray&>       onError;

	// --- Slot equivalents (regular public methods) ---
	void on_pinStateChanged(uint64_t pin, bool state);
	void onDeviceDisconnect();

protected:
	static std::mutex     _mutex;
	static AlpacaDevices  _alpacaDevices;

	bool            _active{false};
	HashType        _hash{0};
	DebugBoardType  _boardType{eUnknownDebugBoard};
	qtac::ByteArray _portName;
	qtac::ByteArray _description;
	qtac::ByteArray _usbDescriptor;
	qtac::ByteArray _macAddress{kMACNotSupported};
	qtac::ByteArray _serialNumber;
	unsigned int    _chipVersion{0};
	PlatformID      _platformID{MICRO_EPM_BOARD_ID_UNKNOWN};
	qtac::ByteArray _helpText;

	qtac::TACDriveThread*  _driveThread{nullptr};
	qtac::TACDriveThread* _serialDriveThread{nullptr};

	TACCommandMap   _commands;
	TACCommands     _commandList;
	qtac::List<qtac::ByteArray> _quickCommandList;
	qtac::ByteArray _lastError;
};

#endif // QTAC_ALPACADEVICE_H
