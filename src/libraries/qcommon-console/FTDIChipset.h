#ifndef FTDICHIPSET_H
#define FTDICHIPSET_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

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
