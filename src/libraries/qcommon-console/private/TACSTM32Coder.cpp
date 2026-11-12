// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "TACSTM32Coder.h"

#include "TACCommandHashes.h"
#include "TACCommands.h"

#include <cstdint>

namespace
{
	const uint8_t kCommandGpioSet{0x01};
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

	switch (CommandStringToHash(encodeMe.toStdString()))
	{
	case kSetPinCommandHash:
		if (arguments.size() >= 2)
		{
			bool state = std::get<bool>(arguments.at(0));
			uint16_t pin = static_cast<uint16_t>(std::get<uint32_t>(arguments.at(1)));
			uint8_t pinBit = static_cast<uint8_t>(1 << pin);

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
