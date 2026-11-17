#ifndef TACCOMMANDS_H
#define TACCOMMANDS_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "StringUtilities.h"

#include <string>

inline const std::string kVersionCommand{"Version"};

inline const std::string kSetPinCommand{"SetPin"};

inline const std::string kButtonAssertTime{"Button Assert Time"};
inline const std::string kSetButtonAssertTime{"Set Button Assert Time"};
inline const std::string kSetButtonAssertTimeAlias{"setbtnassert"};

inline const std::string kPowerKeyDelay{"Power Key Delay"};
inline const std::string kSetPowerKeyDelay{"Set Power Key Delay"};
inline const std::string kSetPowerKeyDelayAlias{"setpkdelay"};

inline const std::string kGetNameCommand{"Get Name"};
inline const std::string kSetNameCommand{"Set Name"};

inline const std::string kGetUUIDCommand{"Get UUID"};

inline const std::string kGetPlatformIDCommand{"Get Platform ID"};

inline const std::string kHelpCommand{"Help"};

inline const std::string kBatteryCommand{"Get Battery"};
inline const std::string kSetBatteryCommand{"Battery"};

inline const std::string kUSB0Command{"Get USB0"};
inline const std::string kSetUSB0Command{"USB0"};

inline const std::string kUSB1Command{"Get USB1"};
inline const std::string kSetUSB1Command{"USB1"};

inline const std::string kPowerKeyCommand{"Get Power Key"};
inline const std::string kSetPowerKeyCommand{"Power Key"};

inline const std::string kVolumeUpCommand{"Get Volume Up"};
inline const std::string kSetVolumeUpCommand{"Volume Up"};

inline const std::string kVolumeDownCommand{"Get Volume Down"};
inline const std::string kSetVolumeDownCommand{"Volume Down"};

inline const std::string kPowerOnCommand{"Power On"};
inline const std::string kPowerOffCommand{"Power Off"};
inline const std::string kBootToFastBootCommand{"Boot To Fastboot"};
inline const std::string kBootToUEFIMenuCommand{"Boot To UEFI"};
inline const std::string kBootToEDLCommand{"Boot to EDL"};

inline const std::string kPrimaryEDLCommand{"Get Primary EDL"};
inline const std::string kSetPrimaryEDLCommand{"Primary EDL"};

inline const std::string kSecondaryEDLCommand{"Get Secondary EDL"};
inline const std::string kSetSecondaryEDLCommand{"Secondary EDL"};

inline const std::string kDisconnectUIM1Command{"Get Disconnect UIM1"};
inline const std::string kSetDisconnectUIM1Command{"Disconnect UIM1"};

inline const std::string kDisconnectUIM2Command{"Get Disconnect UIM2"};
inline const std::string kSetDisconnectUIM2Command{"Disconnect UIM2"};

inline const std::string kDisconnectSDCardCommand{"Get Disconnect SDCARD"};
inline const std::string kSetDisconnectSDCardCommand{"Disconnect SDCARD"};

inline const std::string kForcePSHoldHighCommand{"Get Force PS Hold High"};
inline const std::string kSetForcePSHoldHighCommand{"Force PS Hold High"};

inline const std::string kSecondaryPM_RESIN_NCommand{"Get Secondary PM RESIN N"};
inline const std::string kSetSecondaryPM_RESIN_NCommand{"Secondary PM RESIN N"};

inline const std::string kEUDCommand{"Get Eud"};
inline const std::string kSetEUDCommand{"EUD"};

inline const std::string kHeadsetDisconnectCommand{"Get Headset Disconnect"};
inline const std::string kSetHeadsetDisconnectCommand{"Headset Disconnect"};

inline const std::string kExternalPowerControlCommand{"Get External Power Control"};
inline const std::string kSetExternalPowerControlCommand{"External Power Control"};

inline const std::string kGetResetCountCommand{"Get Reset Count"};
inline const std::string kClearResetCountCommand{"Clear Reset Count"};

inline const std::string kI2CReadRegisterCommand{"I2C Read Register"};
inline const std::string kI2CReadRegisterValueCommand{"I2C Read Register Value"};

inline const std::string kI2CWriteRegisterCommand{"I2C Write Register"};

// PIC32CX Serial Commands
inline const std::string kPIC32CXClearBufferCommand{"echo 1"};
inline const std::string kPIC32CXPlatformIDCommand{"*IDN?"};
inline const std::string kPIC32CXSetPinCommand{"CONF:DIG:ON"};

enum class ArgumentType
{
	UnknownType,
	ByteArray,
	String,
	Int,
	Bool
};

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
	ArgumentType		_argType{ArgumentType::UnknownType};
};

HashType AddCommandToEntries(const std::string& commandToAdd, const std::string& alias, const std::string& helpText);

HashType CommandStringToHash(const std::string& commandString);
oldCommandEntry CommandHashToCommandEntry(HashType commandHash);

void AddHelpTextEntry(std::string &helpText, HashType commandHash);

#endif // TACCOMMANDS_H
