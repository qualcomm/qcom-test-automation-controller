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
// Copyright 2013-2021 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "KingFisherCoder.h"

#include "DiagMessages/DiagMessage.h"

#include <QDataStream>

KingFisherCoder::KingFisherCoder() 
{
	Reset();
}

KingFisherCoder::~KingFisherCoder()
{
}

void KingFisherCoder::Reset()
{
	_parserState = eCollect;
	FrameCoder::Reset();
}

void KingFisherCoder::Decode
(
	const QByteArray& decodeMe
)
{
	if (decodeMe.isEmpty() == false)
	{
		QByteArray::const_iterator decodeChar = decodeMe.begin();
		while (decodeChar != decodeMe.end())
		{
			switch (_parserState)
			{
			case eAA1: HandleStateAA1(*decodeChar); break;
			case eAA2: HandleStateAA2(*decodeChar); break;
			case eAA3: HandleStateAA3(*decodeChar); break;
			case eAB: HandleStateAB(*decodeChar); break;
			case eCollect: HandleStateCollect(*decodeChar); break;
			}

			decodeChar++;
		}
	}
}

const char kPreambleDelimiter(0xAA); // Marks beginning/end of a frame
const char kFrameDelimiter(0xAB); // Marks beginning/end of a message

void KingFisherCoder::HandleStateAA1
(
	char decodeChar
)
{
	if (decodeChar == kPreambleDelimiter)
	{
		_parserState = eAA2;
	}
	else
	{
		if (_frame.isEmpty() == false)
			_frame += kPreambleDelimiter;

		_parserState = eCollect;
	}
}

void KingFisherCoder::HandleStateAA2
(
	char decodeChar
)
{
	if (decodeChar == kPreambleDelimiter)
	{
		_parserState = eAA3;
	}
	else
	{
		if (_frame.isEmpty() == false)
		{
			_frame += kPreambleDelimiter;
			_frame += kPreambleDelimiter;
		}
			
		_parserState = eCollect;
	}
}

void KingFisherCoder::HandleStateAA3
(
	char decodeChar
)
{
	if (decodeChar == kPreambleDelimiter)
	{
		_parserState = eAB;
	}
	else
	{
		if (_frame.isEmpty() == false)
		{
			_frame += kPreambleDelimiter;
			_frame += kPreambleDelimiter;
			_frame += kPreambleDelimiter;
		}
			
		_parserState = eCollect;
	}
}


void KingFisherCoder::HandleStateAB(char decodeChar)
{
	if (decodeChar == kFrameDelimiter)
	{
		if (_frame.isEmpty() == false)
		{
			CompleteTheFrame(_frame);
			_frame.clear();
		}

		_parserState = eCollect;
	}
	else
	{
		if (_frame.isEmpty() == false)
		{
			_frame += kPreambleDelimiter;
			_frame += kPreambleDelimiter;
			_frame += kPreambleDelimiter;
			_frame += kPreambleDelimiter;
		}
			
		_parserState = eCollect;
	}
}

void KingFisherCoder::HandleStateCollect
(
	char decodeChar
)
{
	if (decodeChar == kPreambleDelimiter)
	{
		_parserState = eAA1;
	}
	else
	{
		_frame += decodeChar;
	}
}

const quint16 kKingFisherDBGLOG(0x2923); // 0x2923 Internal - IOE WLAN Debug Log

void KingFisherCoder::CompleteTheFrame
(
	const QByteArray& payload
)
{
	static int count(1);

	if (payload.isEmpty() == false)
	{
		QByteArray completedFrame;
		quint16 logLength = 12 + payload.length();
		QDataStream diagMessageStream(&completedFrame, QIODevice::WriteOnly);
		diagMessageStream.setByteOrder(QDataStream::LittleEndian);
	
		// Diag Header
		diagMessageStream << (quint8) eDIAG_CMD_CODE_LOG;
		diagMessageStream << (quint8) 0; // more
		diagMessageStream << logLength;

		// Log Header
		diagMessageStream << logLength;
		diagMessageStream << (quint16) kKingFisherDBGLOG;

		quint64 mSecs = QDateTime::currentMSecsSinceEpoch();
		diagMessageStream << mSecs;
		completedFrame += payload;
			
		_frameFunction(completedFrame, _protocolInterface);
	}

	count++;
}
