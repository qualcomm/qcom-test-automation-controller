#ifndef TACCOMMANDS_H
#define TACCOMMANDS_H
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

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "StringUtilities.h"

// Qt
#include <QByteArray>
#include <QVariant>

const QByteArray kVersionCommand{QByteArrayLiteral("Version")};

const QByteArray kSetPinCommand{QByteArrayLiteral("SetPin")};

const QByteArray kButtonAssertTime{QByteArrayLiteral("Button Assert Time")};
const QByteArray kSetButtonAssertTime{QByteArrayLiteral("Set Button Assert Time")};
const QByteArray kSetButtonAssertTimeAlias{QByteArrayLiteral("setbtnassert")};

const QByteArray kPowerKeyDelay{QByteArrayLiteral("Power Key Delay")};
const QByteArray kSetPowerKeyDelay{QByteArrayLiteral("Set Power Key Delay")};
const QByteArray kSetPowerKeyDelayAlias{QByteArrayLiteral("setpkdelay")};

const QByteArray kGetNameCommand{QByteArrayLiteral("Get Name")};
const QByteArray kSetNameCommand{QByteArrayLiteral("Set Name")};

const QByteArray kGetUUIDCommand{QByteArrayLiteral("Get UUID")};

const QByteArray kGetPlatformIDCommand{QByteArrayLiteral("Get Platform ID")};

const QByteArray kHelpCommand{QByteArrayLiteral("Help")};

const QByteArray kBatteryCommand{QByteArrayLiteral("Get Battery")};
const QByteArray kSetBatteryCommand{QByteArrayLiteral("Battery")};

const QByteArray kUSB0Command{QByteArrayLiteral("Get USB0")};
const QByteArray kSetUSB0Command{QByteArrayLiteral("USB0")};

const QByteArray kUSB1Command{QByteArrayLiteral("Get USB1")};
const QByteArray kSetUSB1Command{QByteArrayLiteral("USB1")};

const QByteArray kPowerKeyCommand{QByteArrayLiteral("Get Power Key")};
const QByteArray kSetPowerKeyCommand{QByteArrayLiteral("Power Key")};

const QByteArray kVolumeUpCommand{QByteArrayLiteral("Get Volume Up")};
const QByteArray kSetVolumeUpCommand{QByteArrayLiteral("Volume Up")};

const QByteArray kVolumeDownCommand{QByteArrayLiteral("Get Volume Down")};
const QByteArray kSetVolumeDownCommand{QByteArrayLiteral("Volume Down")};

const QByteArray kPowerOnCommand{QByteArrayLiteral("Power On")};
const QByteArray kPowerOffCommand{QByteArrayLiteral("Power Off")};
const QByteArray kBootToFastBootCommand{QByteArrayLiteral("Boot To Fastboot")};
const QByteArray kBootToUEFIMenuCommand{QByteArrayLiteral("Boot To UEFI")};
const QByteArray kBootToEDLCommand{QByteArrayLiteral("Boot to EDL")};

const QByteArray kPrimaryEDLCommand{QByteArrayLiteral("Get Primary EDL")};
const QByteArray kSetPrimaryEDLCommand{QByteArrayLiteral("Primary EDL")};

const QByteArray kSecondaryEDLCommand{QByteArrayLiteral("Get Secondary EDL")};
const QByteArray kSetSecondaryEDLCommand{QByteArrayLiteral("Secondary EDL")};

const QByteArray kDisconnectUIM1Command{QByteArrayLiteral("Get Disconnect UIM1")};
const QByteArray kSetDisconnectUIM1Command{QByteArrayLiteral("Disconnect UIM1")};

const QByteArray kDisconnectUIM2Command{QByteArrayLiteral("Get Disconnect UIM2")};
const QByteArray kSetDisconnectUIM2Command{QByteArrayLiteral("Disconnect UIM2")};

const QByteArray kDisconnectSDCardCommand{QByteArrayLiteral("Get Disconnect SDCARD")};
const QByteArray kSetDisconnectSDCardCommand{QByteArrayLiteral("Disconnect SDCARD")};

const QByteArray kForcePSHoldHighCommand{QByteArrayLiteral("Get Force PS Hold High")};
const QByteArray kSetForcePSHoldHighCommand{QByteArrayLiteral("Force PS Hold High")};

const QByteArray kSecondaryPM_RESIN_NCommand{QByteArrayLiteral("Get Secondary PM RESIN N")};
const QByteArray kSetSecondaryPM_RESIN_NCommand{QByteArrayLiteral("Secondary PM RESIN N")};

const QByteArray kEUDCommand{QByteArrayLiteral("Get Eud")};
const QByteArray kSetEUDCommand{QByteArrayLiteral("EUD")};

const QByteArray kHeadsetDisconnectCommand{QByteArrayLiteral("Get Headset Disconnect")};
const QByteArray kSetHeadsetDisconnectCommand{QByteArrayLiteral("Headset Disconnect")};

const QByteArray kExternalPowerControlCommand{QByteArrayLiteral("Get External Power Control")};
const QByteArray kSetExternalPowerControlCommand{QByteArrayLiteral("External Power Control")};

const QByteArray kGetResetCountCommand{QByteArrayLiteral("Get Reset Count")};
const QByteArray kClearResetCountCommand{QByteArrayLiteral("Clear Reset Count")};

const QByteArray kI2CReadRegisterCommand{QByteArrayLiteral("I2C Read Register")};
const QByteArray kI2CReadRegisterValueCommand{QByteArrayLiteral("I2C Read Register Value")};

const QByteArray kI2CWriteRegisterCommand{QByteArrayLiteral("I2C Write Register")};

// PIC32CX Serial Commands
const QByteArray kPIC32CXClearBufferCommand{QByteArrayLiteral("echo 1")};
const QByteArray kPIC32CXPlatformIDCommand{QByteArrayLiteral("*IDN?")};
const QByteArray kPIC32CXSetPinCommand{QByteArrayLiteral("CONF:DIG:ON")};

struct oldCommandEntry
{
	oldCommandEntry() = default;
	oldCommandEntry(const oldCommandEntry& copyMe) = default;
	oldCommandEntry& operator=(const oldCommandEntry& assignMe) = default;
	~oldCommandEntry() = default;

	QByteArray			_longCommand;
	QByteArray			_compressedCommand;
	HashType			_hash{0};
	QByteArray			_alias;
	QByteArray			_helpText;
	bool				_setter{false};
	QMetaType::Type		_argType{QMetaType::UnknownType};
};

HashType AddCommandToEntries(const QByteArray& commandToAdd, const QByteArray& alias, const QByteArray& helpText);

HashType CommandStringToHash(const QByteArray& commandString);
oldCommandEntry CommandHashToCommandEntry(HashType commandHash);

void AddHelpTextEntry(QString &helpText, HashType commandHash);

#endif // TACCOMMANDS_H
