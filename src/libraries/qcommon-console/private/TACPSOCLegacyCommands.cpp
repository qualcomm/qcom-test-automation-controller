// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "TACPSOCLegacyCommands.h"

// QCommon
#include "AppCore.h"
#include "PSOCPlatformConfiguration.h"

// Qt
#include <QDebug>
#include <QMutexLocker>

namespace
{

// One row per PSoC signal, keyed by the "classic_action" name used in the .tcnf pin map. Keying by
// signal name instead of pin number lets one table serve every platform layout (pin numbers differ
// between platforms, the signal names do not).
//
// The "pin <state> <n>" state is the wire value of the signal: 1 asserts it. For example the current
// scripts power off with "battery 1" -> "pin 1 53", and TAC_POWER_OFF asserted means power off.
// The .tcnf "inverted" flag is not applied here; it does not change what is sent on the wire.
//
// invert = true when the legacy command takes the opposite value. Verified on ALPACA_V3.1, firmware
// 1.4.9.0: "devicePower 0" and "usbDevicePower 0" remove the device under test from USB.
// The other rows come from the Alpaca 3.2.x encoder (libTAC/TACPsocCoder.cpp).
struct LegacySignal
{
	const char*				_classicAction;
	const char*				_command;
	bool					_invert;
};

const LegacySignal kLegacySignals[] =
{
	{"TAC_POWER_OFF",			"devicePower",		true},
	{"TAC_USB0_DIS",			"usbDevicePower",	true},
	{"TAC_USB1_DIS",			"gpio vbusdis1",	false},
	{"TAC_TC_START",			"ttl outputBit 1",	false},		// power key
	{"TAC_VOL_UP",				"gpio volup",		false},
	{"TAC_RESET_PRI",			"ttl outputBit 2",	false},		// volume down
	{"TAC_SW_DWNLD_PRI",		"ttl outputBit 4",	false},		// primary EDL
	{"TAC_FORCE_USB_BOOT_SEC",	"gpio swdwnldsec",	false},		// secondary EDL
	{"TAC_SW_DWNLD_SEC",		"gpio swdwnldsec",	false},
	{"TAC_RESET_SEC",			"gpio resetsec",	false},
	{"TAC_UIM1_DIS",			"ttl outputBit 6",	false},
	{"TAC_UIM2_DIS",			"ttl outputBit 7",	false},
	{"TAC_SDCARD_DISC",			"gpio sdcarddis",	false},
	{"TAC_FORCE_PS_HOLD",		"gpio pshold",		false},
	// EUD and headset have no command in the legacy firmware help list; they stay unmapped.
};

const LegacySignal* findSignal(const QString& classicAction)
{
	for (const auto& legacySignal: kLegacySignals)
	{
		if (classicAction.compare(QLatin1String(legacySignal._classicAction), Qt::CaseInsensitive) == 0)
			return &legacySignal;
	}

	return nullptr;
}

void logLine(const QString& message)
{
	AppCore::writeToApplicationLogLine(message);
	qWarning().noquote() << message;
}

} // namespace

bool PSOCLegacyCommands::compiledIn()
{
#ifdef QTAC_NO_PSOC_LEGACY_COMMANDS
	return false;
#else
	return true;
#endif
}

PSOCLegacyCommands::Mode PSOCLegacyCommands::mode()
{
	static const Mode kMode = []() -> Mode
	{
		if (compiledIn() == false)
			return eOff;

		const QString value = qEnvironmentVariable("TAC_PSOC_LEGACY_COMMANDS").trimmed().toLower();
		if (value == "on" || value == "1" || value == "force")
			return eForceOn;
		if (value == "off" || value == "0")
			return eOff;

		return eAuto;
	}();

	return kMode;
}

void PSOCLegacyCommands::configure
(
	_PSOCPlatformConfiguration* platformConfiguration,
	const QString& firmwareVersion,
	uint firmwareMinor,
	const QByteArray& portName
)
{
	if (mode() == eOff || platformConfiguration == Q_NULLPTR)
		return;

	{
		QMutexLocker lock(&_mutex);

		_portName = portName;
		_pins.clear();

		for (const auto& pinData: platformConfiguration->getAllPins())
		{
			const LegacySignal* legacySignal = findSignal(pinData._classicAction);
			if (legacySignal != nullptr)
				_pins.insert(pinData._pin, Target{QByteArray(legacySignal->_command), legacySignal->_invert});
		}
	}

	if (mode() == eForceOn)
	{
		activate("forced by TAC_PSOC_LEGACY_COMMANDS");
	}
	else if (firmwareVersion.trimmed().isEmpty() == false && firmwareMinor < kFirstFirmwareMinorWithPin)
	{
		activate(QString("firmware %1 predates the 'pin' command (minor %2 < %3)")
			.arg(firmwareVersion.trimmed()).arg(firmwareMinor).arg(kFirstFirmwareMinorWithPin));
	}
}

QByteArray PSOCLegacyCommands::translate
(
	bool state,
	PinID pin
) const
{
	if (_active.load() == false)
		return QByteArray();

	QMutexLocker lock(&_mutex);

	auto target = _pins.constFind(pin);
	if (target == _pins.constEnd())
	{
		if (_reportedUnmapped.contains(pin) == false)
		{
			_reportedUnmapped.insert(pin);
			logLine(QString("%1: legacy firmware has no command for pin %2; sending 'pin' unchanged")
				.arg(QString::fromLatin1(_portName)).arg(pin));
		}

		return QByteArray();
	}

	const bool value = target->_invert ? !state : state;

	return target->_command + (value ? " 1" : " 0") + "\r";
}

void PSOCLegacyCommands::onCommandRejected
(
	const QByteArray& echoedCommand
)
{
	if (_active.load() == true || mode() != eAuto)
		return;

	if (echoedCommand.trimmed().startsWith("pin "))
		activate(QString("firmware rejected '%1'").arg(QString::fromLatin1(echoedCommand.trimmed())));
}

void PSOCLegacyCommands::activate
(
	const QString& reason
)
{
	if (_active.exchange(true) == true)
		return;

	int mapped{0};
	QByteArray portName;
	{
		QMutexLocker lock(&_mutex);
		mapped = _pins.count();
		portName = _portName;
	}

	logLine(QString("Warning: %1 uses legacy PSoC commands (%2); %3 pins mapped. Please update the board firmware.")
		.arg(QString::fromLatin1(portName), reason).arg(mapped));
}
