// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef STM32PLATFORMCONFIGURATION_H
#define STM32PLATFORMCONFIGURATION_H

#include "QCommonConsoleGlobal.h"

#include "PlatformConfiguration.h"
#include "STM32PinData.h"


class QCOMMONCONSOLE_EXPORT _STM32PlatformConfiguration: public _PlatformConfiguration
{
public:
	_STM32PlatformConfiguration();
	~_STM32PlatformConfiguration();

	void initialize();

	STM32PinData getPinData(PinID pin);

	STM32PinList getAllPins() const;
	STM32PinList getActivePins() const;

	virtual Pins getPins();

	bool getPinEnableState(const PinID pin) const;
	void setPinEnableState(HashType hash, bool newState);

	bool getPinInvertedState(const PinID pin) const;
	void setPinInvertedState(HashType hash, bool newState);

	QString getPinLabel(const PinID pin) const;
	void setPinLabel(HashType hash, const QString& pinLabel);

	QString getPinTooltip(const PinID pin) const;
	void setPinTooltip(HashType hash, const QString& pinTooltip);

	QString getPinCommand(const PinID pin) const;
	void setPinCommand(HashType hash, const QString& pinCommand);

	CommandGroups getPinGroup(const PinID pin) const;
	void setPinGroup(HashType hash, const CommandGroups pinGroup);

	QString getTabName(const PinID pin) const;
	void setTabName(HashType hash, const QString& tabName);

	QPoint getPinCellLocation(const PinID pin) const;
	void setPinCellLocation(HashType hash, const QPoint& cellLocation);

protected:
	virtual void cascadeTabDelete(const QString& deleteMe);
	virtual void cascadeTabRename(const QString& oldName, const QString& newName);

	virtual bool read(QJsonObject& parentLevel);
	virtual void write(QJsonObject& parentLevel);

private:
	STM32PinEntries					_pinEntries;
};

#endif // STM32PLATFORMCONFIGURATION_H
