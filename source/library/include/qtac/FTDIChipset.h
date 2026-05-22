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

// Authors: msimpson, biswroy

#ifndef QTAC_FTDICHIPSET_H
#define QTAC_FTDICHIPSET_H

#include <qtac/CharBit.h>
#include <qtac/FTDIPinSet.h>
#include <qtac/PlatformID.h>
#include <qtac/StringUtilities.h>
#include <qtac/ByteArray.h>
#include <qtac/List.h>
#include <qtac/Map.h>

#include <cstdint>
#include <memory>

namespace qtac {

class _FTDIChipset;

using FTDIChipset    = std::shared_ptr<_FTDIChipset>;
using FTDIChipsetList = qtac::List<FTDIChipset>;
using FTDIChipsetMap  = qtac::Map<HashType, FTDIChipset>;

class _FTDIChipset
{
public:
	_FTDIChipset(const _FTDIChipset&) = delete;
	_FTDIChipset();
	~_FTDIChipset();

	static qtac::String ftidStatusToString(unsigned long status);

	static uint32_t getDeviceCount();
	static FTDIChipset getDevice(uint32_t deviceIndex);
	static FTDIChipset getDevice(const qtac::ByteArray& portName);
	static qtac::ByteArray normalizeSerialNumber(const qtac::ByteArray& segmentSerialNumber);

	bool open(FTDIPinSets pinsets);
	bool isOpen();
	void close();

	PlatformID platformID();
	void setPlatformID(PlatformID platformID);

	qtac::String versionString();
	qtac::String firmwareString();

	void reset()
	{
		_active = false;
		_ftdiChipsetList.clear();
	}

	bool active() const { return _active; }

	HashType hash();
	qtac::ByteArray serialNumber();
	bool newDevice();

	qtac::ByteArray portName() const { return _portName; }
	qtac::ByteArray usbDescriptor() const { return _usbDescriptor; }
	qtac::ByteArray lastError() const { return _lastError; }

	qtac::ByteArray aSerialNumber();
	void setASerialNumber(const qtac::ByteArray& sn);

	qtac::ByteArray bSerialNumber();
	void setBSerialNumber(const qtac::ByteArray& sn);

	qtac::ByteArray cSerialNumber();
	void setCSerialNumber(const qtac::ByteArray& sn);

	qtac::ByteArray dSerialNumber();
	void setDSerialNumber(const qtac::ByteArray& sn);

	bool write(uint8_t pin, bool state);

	// made public to allow traversal code to set these directly
	bool            _active{false};
	qtac::ByteArray _usbDescriptor;
	qtac::ByteArray _lastError;

private:
	static HashType hash(const qtac::ByteArray& serialNumber);
	void setSerialNumber(const qtac::ByteArray& serialNumber);
	static unsigned long long setCustomVIDPID();
	static bool hasDevice(HashType portHash);
	void setupHash(const qtac::ByteArray& segmentSerialNumber);
	void setupPortName();
	static PlatformID nameToPlatform(const qtac::ByteArray& deviceName);
	static void linuxTraversal();
	static void windowsTraversal();

	HashType        _hash{0};
	PlatformID      _platformID{ALPACA_LITE_ID};
	bool            _new{true};
	CharBit         _aPins;
	CharBit         _bPins;
	CharBit         _cPins;
	CharBit         _dPins;

	qtac::ByteArray _serialNumber;
	qtac::ByteArray _portName;

	qtac::ByteArray _aSerialNumber;
	void*           _aHandle{nullptr};

	qtac::ByteArray _bSerialNumber;
	void*           _bHandle{nullptr};

	qtac::ByteArray _cSerialNumber;
	void*           _cHandle{nullptr};

	qtac::ByteArray _dSerialNumber;
	void*           _dHandle{nullptr};

	static FTDIChipsetList _ftdiChipsetList;
};

} // namespace qtac

// Bring into global namespace for convenience
using qtac::_FTDIChipset;
using qtac::FTDIChipset;
using qtac::FTDIChipsetList;
using qtac::FTDIChipsetMap;

#endif // QTAC_FTDICHIPSET_H
