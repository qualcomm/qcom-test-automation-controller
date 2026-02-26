// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef PIC32CXPLATFORMCONFIGURATION_H
#define PIC32CXPLATFORMCONFIGURATION_H

#include "CommandGroup.h"
#include "PlatformConfiguration.h"

const PlatformID kMaxPIC32CXPlatformId(99999);


struct PIC32CXPinData
{
	PIC32CXPinData() = default;
	PIC32CXPinData(const PIC32CXPinData& copyMe) = default;
	PIC32CXPinData(PinID setPin)
	{
		_hash = PIC32CXPinData::makePIC32CXHash(setPin);
	}

	void clear()
	{
		*this = PIC32CXPinData();
	}

	static HashType makePIC32CXHash(PinID setPin)
	{
		HashType result{0};

		QString pinStr = QString::number(setPin);

		result = strHash(pinStr);

		return result;
	}

	HashType					_hash{0};

	PinID						_setPin{static_cast<PinID>(-1)};
	bool						_enabled{false};
	QString						_pinLabel;
	QString						_pinTooltip;
	bool						_inverted{false};
	QString						_pinCommand;
	CommandGroups				_commandGroup{eUnknownCommandGroup};
	QPoint						_cellLocation{QPoint(-1,-1)};
	QString						_tabName{"General"};
};

typedef QMap<PinID, PIC32CXPinData> PIC32CXPinEntries;
typedef QList<PIC32CXPinData> PIC32CXPinList;

class _PIC32CXPlatformConfiguration;

class QCOMMONCONSOLE_EXPORT _PIC32CXPlatformConfiguration:
	public _PlatformConfiguration
{
public:
	_PIC32CXPlatformConfiguration();
	virtual ~_PIC32CXPlatformConfiguration();

	virtual Pins getPins();

	PIC32CXPinList getAllPins();
	PIC32CXPinList getActivePins();

	bool getPinEnableState(const PinID pinId) const;
	void setPinEnableState(const HashType hash, bool newState);

	bool getPinInvertedState(const PinID pinId) const;
	void setPinInvertedState(const HashType hash, bool newState);

	QString getPinLabel(const PinID pinId) const;
	void setPinLabel(const HashType hash, const QString& pinLabel);

	QString getPinTooltip(const PinID pinId) const;
	void setPinTooltip(const HashType hash, const QString& pinTooltip);

	QString getPinCommand(const PinID pinId) const;
	void setPinCommand(const HashType hash, const QString& pinCommand);

	CommandGroups getPinGroup(const PinID pinId) const;
	void setPinGroup(const HashType hash, const CommandGroups pinGroup);

	QString getTabName(const PinID pinId) const;
	void setTabName(const HashType hash, const QString& tabName);

	QPoint getPinCellLocation(const PinID pinId) const;
	void setPinCellLocation(const PinID pinId, const QPoint& cellLocation);

	PinID bitFromSetPin(PinID setPin);

protected:
	virtual void cascadeTabDelete(const QString& deleteMe);
	virtual void cascadeTabRename(const QString& oldName, const QString& newName);

	virtual bool read(QJsonObject& parentLevel);
	virtual void write(QJsonObject& parentLevel);

private:
	void initialize();

	PIC32CXPinEntries               _pinEntries;
};
#endif // PIC32CXPLATFORMCONFIGURATION_H
