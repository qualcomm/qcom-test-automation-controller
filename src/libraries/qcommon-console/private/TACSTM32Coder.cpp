// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "TACSTM32Coder.h"

#include "TACCommandHashes.h"
#include "TACCommands.h"

namespace
{
	const quint8 kCommandGpioSet{0x01};
	const int kReportPayloadSize{32};
}

TACSTM32Coder::TACSTM32Coder() :
	FrameCoder()
{
}

TACSTM32Coder::~TACSTM32Coder()
{
}

void TACSTM32Coder::reset()
{
	FrameCoder::reset();

	_gpioState = 0;
}

void TACSTM32Coder::decode
(
	const QByteArray& decodeMe
)
{
	Q_UNUSED(decodeMe)

	// BugHopper V2 has no framed protocol to decode responses from
}

QByteArray TACSTM32Coder::encode
(
	const QByteArray& encodeMe,
	const Arguments& arguments
)
{
	QByteArray result;

	switch (CommandStringToHash(encodeMe))
	{
	case kSetPinCommandHash:
		if (arguments.count() >= 2)
		{
			bool state = arguments.at(0).toBool();
			quint16 pin = static_cast<quint16>(arguments.at(1).toUInt());
			quint8 pinBit = static_cast<quint8>(1 << pin);

			_gpioState = state ? (_gpioState | pinBit) : (_gpioState & ~pinBit);

			result = QByteArray(kReportPayloadSize + 1, char(0));
			result[1] = static_cast<char>(kCommandGpioSet);
			result[2] = static_cast<char>(_gpioState);
			result[3] = static_cast<char>(pinBit);
		}
		break;

	default:
		break;
	}

	return result;
}
