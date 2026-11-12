#ifndef TACCOMMANDS_H
#define TACCOMMANDS_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "StringUtilities.h"

// Qt
#include <QMetaType>

#include <string>

const std::string kVersionCommand{"Version"};

const std::string kSetPinCommand{"SetPin"};

const std::string kButtonAssertTime{"Button Assert Time"};
const std::string kSetButtonAssertTime{"Set Button Assert Time"};
const std::string kSetButtonAssertTimeAlias{"setbtnassert"};

const std::string kPowerKeyDelay{"Power Key Delay"};
const std::string kSetPowerKeyDelay{"Set Power Key Delay"};
const std::string kSetPowerKeyDelayAlias{"setpkdelay"};

const std::string kGetNameCommand{"Get Name"};
const std::string kSetNameCommand{"Set Name"};

const std::string kGetUUIDCommand{"Get UUID"};

const std::string kGetPlatformIDCommand{"Get Platform ID"};

const std::string kHelpCommand{"Help"};

const std::string kBatteryCommand{"Get Battery"};
const std::string kSetBatteryCommand{"Battery"};

const std::string kUSB0Command{"Get USB0"};
const std::string kSetUSB0Command{"USB0"};

const std::string kUSB1Command{"Get USB1"};
const std::string kSetUSB1Command{"USB1"};

const std::string kPowerKeyCommand{"Get Power Key"};
const std::string kSetPowerKeyCommand{"Power Key"};

const std::string kVolumeUpCommand{"Get Volume Up"};
const std::string kSetVolumeUpCommand{"Volume Up"};

const std::string kVolumeDownCommand{"Get Volume Down"};
const std::string kSetVolumeDownCommand{"Volume Down"};

const std::string kPowerOnCommand{"Power On"};
const std::string kPowerOffCommand{"Power Off"};
const std::string kBootToFastBootCommand{"Boot To Fastboot"};
const std::string kBootToUEFIMenuCommand{"Boot To UEFI"};
const std::string kBootToEDLCommand{"Boot to EDL"};

const std::string kPrimaryEDLCommand{"Get Primary EDL"};
const std::string kSetPrimaryEDLCommand{"Primary EDL"};

const std::string kSecondaryEDLCommand{"Get Secondary EDL"};
const std::string kSetSecondaryEDLCommand{"Secondary EDL"};

const std::string kDisconnectUIM1Command{"Get Disconnect UIM1"};
const std::string kSetDisconnectUIM1Command{"Disconnect UIM1"};

const std::string kDisconnectUIM2Command{"Get Disconnect UIM2"};
const std::string kSetDisconnectUIM2Command{"Disconnect UIM2"};

const std::string kDisconnectSDCardCommand{"Get Disconnect SDCARD"};
const std::string kSetDisconnectSDCardCommand{"Disconnect SDCARD"};

const std::string kForcePSHoldHighCommand{"Get Force PS Hold High"};
const std::string kSetForcePSHoldHighCommand{"Force PS Hold High"};

const std::string kSecondaryPM_RESIN_NCommand{"Get Secondary PM RESIN N"};
const std::string kSetSecondaryPM_RESIN_NCommand{"Secondary PM RESIN N"};

const std::string kEUDCommand{"Get Eud"};
const std::string kSetEUDCommand{"EUD"};

const std::string kHeadsetDisconnectCommand{"Get Headset Disconnect"};
const std::string kSetHeadsetDisconnectCommand{"Headset Disconnect"};

const std::string kExternalPowerControlCommand{"Get External Power Control"};
const std::string kSetExternalPowerControlCommand{"External Power Control"};

const std::string kGetResetCountCommand{"Get Reset Count"};
const std::string kClearResetCountCommand{"Clear Reset Count"};

const std::string kI2CReadRegisterCommand{"I2C Read Register"};
const std::string kI2CReadRegisterValueCommand{"I2C Read Register Value"};

const std::string kI2CWriteRegisterCommand{"I2C Write Register"};

// PIC32CX Serial Commands
const std::string kPIC32CXClearBufferCommand{"echo 1"};
const std::string kPIC32CXPlatformIDCommand{"*IDN?"};
const std::string kPIC32CXSetPinCommand{"CONF:DIG:ON"};

struct oldCommandEntry
{
	oldCommandEntry() = default;
	oldCommandEntry(const oldCommandEntry& copyMe) = default;
	oldCommandEntry& operator=(const oldCommandEntry& assignMe) = default;
	~oldCommandEntry() = default;

	std::string			_longCommand;
	std::string			_compressedCommand;
	HashType			_hash{0};
	std::string			_alias;
	std::string			_helpText;
	bool				_setter{false};
	QMetaType::Type		_argType{QMetaType::UnknownType};
};

HashType AddCommandToEntries(const std::string& commandToAdd, const std::string& alias, const std::string& helpText);

HashType CommandStringToHash(const std::string& commandString);
oldCommandEntry CommandHashToCommandEntry(HashType commandHash);

void AddHelpTextEntry(std::string &helpText, HashType commandHash);

#endif // TACCOMMANDS_H
