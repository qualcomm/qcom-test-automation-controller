#ifndef FTDICHIPSET_H
#define FTDICHIPSET_H
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

// Author: msimpson, biswroy

// QCommon
#include "CharBit.h"
#include "FTDIPinSet.h"
#include "PlatformID.h"
#include "StringUtilities.h"

#include <QSharedPointer>
#include <QMap>

class _FTDIChipset;

typedef QSharedPointer<_FTDIChipset> FTDIChipset;
typedef QList<FTDIChipset> FTDIChipsetList;
typedef QMap<HashType, FTDIChipset> FTDIChipsetMap;

#include "QCommonConsoleGlobal.h"

class QCOMMONCONSOLE_EXPORT _FTDIChipset
{
public:

	_FTDIChipset(const _FTDIChipset& copyMe) = delete;

	_FTDIChipset();

	static QString ftidStatusToString(unsigned long status);

	static quint32 getDeviceCount();
	static FTDIChipset getDevice(quint32 deviceIndex);
	static FTDIChipset getDevice(const QByteArray& portName);
	static QByteArray normalizeSerialNumber(const QByteArray& segmentSerialNumber);

	bool open(FTDIPinSets pinsets);
	bool isOpen();
	void close();

	PlatformID platformID();
	void setPlatformID(PlatformID platformID);

	QString versionString();
	QString firmwareString();

	void reset()
	{
		_active = false;
		_ftdiChipsetList.clear();
	}

	bool active()
	{
		return _active;
	}

	HashType hash();
	QByteArray serialNumber();
	bool newDevice();

	QByteArray portName()
	{
		return _portName;
	}

	QByteArray usbDescriptor()
	{
		return _usbDescriptor;
	}

	QByteArray lastError()
	{
		return _lastError;
	}

	QByteArray aSerialNumber();
	void setASerialNumber(const QByteArray& aSerialNumber);

	QByteArray bSerialNumber();
	void setBSerialNumber(const QByteArray& bSerialNumber);

	QByteArray cSerialNumber();
	void setCSerialNumber(const QByteArray& cSerialNumber);

	QByteArray dSerialNumber();
	void setDSerialNumber(const QByteArray& dSerialNumber);

	bool write(quint8 pin, bool state);

private:
	static HashType hash(const QByteArray& serialNumber);

	void setSerialNumber(const QByteArray& serialNumber);
	static unsigned long long setCustomVIDPID();

	static bool hasDevice(HashType portHash);
	void setupHash(const QByteArray& segmentSerialNumber);
	void setupPortName();
	static PlatformID nameToPlatform(const QByteArray& deviceName);

	static void linuxTraversal();
	static void windowsTraversal();

	HashType			_hash{0};
	PlatformID			_platformID{ALPACA_LITE_ID};
	bool				_new{true};
	bool				_active{false};
	CharBit				_aPins;
	CharBit				_bPins;
	CharBit				_cPins;
	CharBit				_dPins;

	QByteArray			_serialNumber;
	QByteArray			_portName;
	QByteArray			_usbDescriptor;

	QByteArray			_aSerialNumber;
	void*				_aHandle{Q_NULLPTR};

	QByteArray			_bSerialNumber;
	void*				_bHandle{Q_NULLPTR};

	QByteArray			_cSerialNumber;
	void*				_cHandle{Q_NULLPTR};

	QByteArray			_dSerialNumber;
	void*				_dHandle{Q_NULLPTR};

	QByteArray          _lastError;

	static FTDIChipsetList		_ftdiChipsetList;
};


#endif // FTDICHIPSET_H
