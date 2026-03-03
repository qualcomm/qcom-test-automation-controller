#ifndef PSOCPLATFORMCONFIGURATION_H
#define PSOCPLATFORMCONFIGURATION_H
// Confidential and Proprietary Qualcomm Technologies, Inc.

// NO PUBLIC DISCLOSURE PERMITTED:  Please report postings of this software on public servers or websites
// to: DocCtrlAgent@qualcomm.com.

// RESTRICTED USE AND DISCLOSURE:
// This software contains confidential and proprietary information and is not to be used, copied, reproduced, modified
// or distributed, in whole or in part, nor its contents revealed in any manner, without the express written permission
// of Qualcomm Technologies, Inc.

// Qualcomm is a trademark of QUALCOMM Incorporated, registered in the United States and other countries. All
// QUALCOMM Incorporated trademarks are used with permission.

// This software may be subject to U.S. and international export, re-export, or transfer laws.  Diversion contrary to U.S.
// and international law is strictly prohibited.

// Qualcomm Technologies, Inc.
// 5775 Morehouse Drive
// San Diego, CA 92121 U.S.A.
// Copyright 2022-2025 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "PlatformConfiguration.h"
#include "CommandGroup.h"

const PlatformID kMaxPSOCPlatformId(255);

struct PSOCPinData
{
	PSOCPinData() = default;
	PSOCPinData(const PSOCPinData& copyMe) = default;
	PSOCPinData(PinID pin)
	{
		_pin = pin;
		_hash = _pin;
	}

	void clear()
	{
		*this = PSOCPinData();
	}

	// TODO: PinID made compatible with HashType, under condition that PSOC will
	// have a default pin as 0 instead of -1 previously.
	PinID						_pin{0};
	HashType					_hash{0};
	bool						_enabled{true};
	QString						_pinLabel;
	QString						_pinTooltip;
	bool						_initialValue{false};
	int							_initializationPriority{0};
	bool						_inverted{false};
	QString						_pinCommand;
	CommandGroups				_commandGroup{eUnknownCommandGroup};
	QString						_classicAction;
	QPoint						_cellLocation{QPoint(-1,-1)};
	QString						_tabName;
};

typedef QMap<PinID, PSOCPinData> PSOCPinEntries;
typedef QList<PSOCPinData> PSOCPinList;

class _PSOCPlatformConfiguration;

class QCOMMONCONSOLE_EXPORT _PSOCPlatformConfiguration:
	public _PlatformConfiguration
{
public:
	_PSOCPlatformConfiguration();
	virtual ~_PSOCPlatformConfiguration();

	virtual Pins getPins();

	PSOCPinList getAllPins();
	PSOCPinList getActivePins();

	bool getPinEnableState(const PinID pinId) const;
	void setPinEnableState(const PinID pinId, bool newState);

	bool getInitialPinValue(const PinID pinId) const;
	void setInitialPinValue(const PinID pinId, bool newState);

	quint64 getPinInitializationPriority(const PinID pinId) const;
	void setPinInitializationPriority(const PinID pinId, const int priority);

	bool getPinInvertedState(const PinID pinId) const;
	void setPinInvertedState(const PinID pinId, bool newState);

	QString getPinLabel(const PinID pinId) const;
	void setPinLabel(const PinID pinId, const QString& pinLabel);

	QString getPinTooltip(const PinID pinId) const;
	void setPinTooltip(const PinID pinId, const QString& pinTooltip);

	QString getPinCommand(const PinID pinId) const;
	void setPinCommand(const PinID pinId, const QString& pinCommand);

	CommandGroups getPinGroup(const PinID pinId) const;
	void setPinGroup(const PinID pinId, const CommandGroups commandGroup);

	QString getTabName(const PinID pinId) const;
	void setTabName(const PinID pinId, const QString tabName);

	QString getClassicAction(const PinID pinId) const;
	void setClassicAction(const PinID pinId, const QString& classicAction);

	QPoint getPinCellLocation(const PinID pinId) const;
	void setPinCellLocation(const PinID pinId, const QPoint& cellLocation);

protected:
	virtual void cascadeTabDelete(const QString& deleteMe);
	virtual void cascadeTabRename(const QString& oldName, const QString& newName);

	virtual bool read(QJsonObject& parentLevel);
	virtual void write(QJsonObject& parentLevel);

private:
	PSOCPinEntries				_pinEntries;

	static void initialize();
	static PSOCPinEntries		_classicActions;
};

#endif // PSOCPLATFORMCONFIGURATION_H
