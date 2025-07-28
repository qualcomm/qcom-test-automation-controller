// Confidential and Proprietary – Qualcomm Technologies, Inc.

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
// Copyright ©2022 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

// Author: msimpson

#include "TACLiteCoder.h"
#include "TACCommands.h"
#include "TacCommandHashes.h"

static quint32 designToPin(quint32 designPin)
{
	switch (designPin)
	{
	// Bus A
	case 16: return 0;
	case 17: return 1;
	case 18: return 2;
	case 19: return 3;
	case 21: return 4;
	case 22: return 5;
	case 23: return 6;
	case 24: return 7;

	// Bus B
	case 26: return 8;
	case 27: return 9;
	case 28: return 10;
	case 29: return 11;
	case 30: return 12;
	case 32: return 13;
	case 33: return 14;
	case 34: return 15;

	// Bus C
	case 38: return 16;
	case 39: return 17;
	case 40: return 18;
	case 41: return 19;
	case 43: return 20;
	case 44: return 21;
	case 45: return 22;
	case 46: return 23;

	// Bus D
	case 48: return 24;
	case 52: return 25;
	case 53: return 26;
	case 54: return 27;
	case 55: return 28;
	case 57: return 29;
	case 58: return 30;
	case 59: return 31;
	}

	return 0xFFFFFFF;
}

TacLiteCoder::TacLiteCoder() :
	FrameCoder()
{
}

TacLiteCoder::~TacLiteCoder()
{
}

void TacLiteCoder::reset()
{
	FrameCoder::reset();
}

void TacLiteCoder::decode
(
	const QByteArray& decodeMe
)
{
	Q_UNUSED(decodeMe)
}

QByteArray TacLiteCoder::encode
(
	const QByteArray& encodeMe,
	const Arguments &arguments
)
{
	Q_UNUSED(arguments)

	QByteArray result(encodeMe);

	switch (CommandStringToHash(encodeMe))
	{
	case kSetPinCommandHash:
		result = QByteArray::number(arguments.at(1).toUInt());
		break;

	default:
		result = "Invalid";
		break;
	}

	return result;
}
