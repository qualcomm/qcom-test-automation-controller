// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef TACPSOCLEGACYCOMMANDS_H
#define TACPSOCLEGACYCOMMANDS_H

#include "QCommonConsoleGlobal.h"

// QCommon
#include "PinID.h"

// Qt
#include <QByteArray>
#include <QHash>
#include <QMutex>
#include <QSet>
#include <QString>

#include <atomic>

class _PSOCPlatformConfiguration;

// Compatibility layer for PSoC firmware that predates the generic "pin <state> <n>" command
// (firmware older than 1.x.15, for example 1.4.9.0 on ALPACA_V3.1 boards). That firmware
// answers "pin" with "Command not recognized" and only understands the named commands used by
// Alpaca 3.2.x: devicePower, usbDevicePower, gpio <name>, ttl outputBit <n>.
//
// The module is self-contained. TACPSOCCoder asks translate() for every pin command; while legacy
// mode is inactive it returns an empty array and the normal "pin" encoding is sent unchanged, so
// boards with current firmware are not affected.
//
// On/off:
//   build time: CMake option QTAC_PSOC_LEGACY_COMMANDS (default ON). When OFF, legacy mode never activates.
//   run time:   environment variable TAC_PSOC_LEGACY_COMMANDS = auto (default) | on | off
//
// Activation in auto mode:
//   1. when the device opens, if firmwarePredatesPin() is true for the parsed major and minor version;
//   2. as a safety net, the first time the firmware rejects a "pin" command (covers firmware whose
//      version string cannot be parsed). The rejected command itself is not resent; it is logged.
//
// Extending: to support another signal, add one row to kLegacySignals in TACPSOCLegacyCommands.cpp.
class QCOMMONCONSOLE_EXPORT PSOCLegacyCommands
{
public:
	enum Mode
	{
		eAuto,
		eForceOn,
		eOff
	};

	// First firmware with the "pin" command is 1.x.15. The version reads major.chip.minor.revision;
	// the chip field identifies the part, not the release, so it is not part of the comparison.
	static constexpr uint kFirstFirmwareMajorWithPin{1};
	static constexpr uint kFirstFirmwareMinorWithPin{15};

	// True when major.minor is older than the first firmware with "pin". The major version is
	// compared first, so a later major release such as 2.0.1 is never treated as legacy.
	static bool firmwarePredatesPin(uint firmwareMajor, uint firmwareMinor);

	PSOCLegacyCommands() = default;

	// Called once the platform configuration is known, before any pin command is sent.
	void configure(_PSOCPlatformConfiguration* platformConfiguration, const QString& firmwareVersion,
		uint firmwareMajor, uint firmwareMinor, const QByteArray& portName);

	bool active() const
	{
		return _active.load();
	}

	// Legacy firmware command for "pin <state> <pin>", or an empty array to keep the normal encoding.
	QByteArray translate(bool state, PinID pin) const;

	// Called by the coder when the firmware answers "Command not recognized".
	void onCommandRejected(const QByteArray& echoedCommand);

	static Mode mode();
	static bool compiledIn();

private:
	struct Target
	{
		QByteArray				_command;
		bool					_invert{false};
	};

	void activate(const QString& reason);

	mutable QMutex				_mutex;
	QHash<PinID, Target>		_pins;
	mutable QSet<PinID>			_reportedUnmapped;
	QByteArray					_portName;
	std::atomic<bool>			_active{false};
};

#endif // TACPSOCLEGACYCOMMANDS_H
