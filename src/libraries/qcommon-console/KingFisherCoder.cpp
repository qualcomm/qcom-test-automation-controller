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
