
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
// Copyright © 2013-2015 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	$Id: //depot/QDL/QCommon/NonHDLCFrameCoder.cpp#6 $
	$Header: //depot/QDL/QCommon/NonHDLCFrameCoder.cpp#6 $
	$Date: 2016/10/06 $
	$DateTime: 2016/10/06 15:16:21 $
	$Change: 910705 $
	$File: //depot/QDL/QCommon/NonHDLCFrameCoder.cpp $
	$Revision: #6 $
	%Author: msimpson %
*/

#include "NonHDLCFrameCoder.h"

#include <QtCore/QDataStream>

const quint8 kFrameDelimiter(0x7E); // Marks beginning/end of a frame

NonHDLCFrameCoder::NonHDLCFrameCoder()
{
}

void NonHDLCFrameCoder::Reset()
{
	FrameCoder::Reset();
}

void NonHDLCFrameCoder::Decode
(
	const QByteArray& decodeMe
)
{
	if (decodeMe.isEmpty() == false)
	{
		bool processFrames(true);

		if (_frame.isEmpty() == false)
			_frame += decodeMe;
		else
			_frame = decodeMe;

      if (_frame.isEmpty())
         return;

		while (processFrames == true)
		{
			QByteArray badFrameData;

			while (_frame.isEmpty() == false && _frame.at(0) != kFrameDelimiter)
			{
				badFrameData += _frame.at(0);
				_frame.remove(0, 1);
			}

			if (badFrameData.isEmpty() == false)
			{
				ErrorParameters errors;

				//NonHDLCFrameCoder::_logger.Log("Dumping Bad Frame Data while looking for delimiter:");
				//NonHDLCFrameCoder::_logger.LogInHex(badFrameData, true, 16);
				//NonHDLCFrameCoder::_logger.Log("\n", false);

				_badFrameFunction(badFrameData, errors, _protocolInterface);
				badFrameData.clear();
			}

			if (_frame.size() >= 4)
			{
				int version = _frame.at(1);
				switch (version)
				{
				case 1:
					processFrames = HandleVersion1FrameData();
					break;

				default:
					HandleBadFrameDataVersion();
					break;
				}
				
				while (_frame.size() > 1 && (_frame.at(0) == kFrameDelimiter && _frame.at(1) == kFrameDelimiter)) // There is supposed to be an end frame delimiter, but some implementations omit it
					_frame.remove(0, 1); // If we find one, remove it
			}
			else
				processFrames = false;

		} // while (processFrames == true)
	}
}

QByteArray NonHDLCFrameCoder::Encode
(
	const QByteArray& encodeMe
)
{
	return NonHDLCFrameCoder::EncodeFrame(encodeMe);
}

QByteArray NonHDLCFrameCoder::EncodeFrame(const QByteArray& encodeMe)
{
	QByteArray result(encodeMe);
	QDataStream dataStream(&result, QIODevice::WriteOnly);

	dataStream.setByteOrder(QDataStream::LittleEndian);

	dataStream << kFrameDelimiter;
	dataStream << (quint8) 1;
	dataStream << (quint16) encodeMe.length();
	dataStream.writeRawData(encodeMe.constData(), encodeMe.length());
	dataStream << kFrameDelimiter;

	return result;
}

bool NonHDLCFrameCoder::HandleVersion1FrameData()
{
	bool result(false);

	char lengthBuffer[2];

	lengthBuffer[0] = _frame.at(2);
	lengthBuffer[1] = _frame.at(3);
	int payloadLen = *((ushort*)(&lengthBuffer[0]));
	int frameSize = _frame.count() - 4; // 4 accounts for the size of the header
	if (frameSize >= payloadLen) // we have a complete frame?
	{
		const char* payload = (char*)_frame.constData() + 4;
		QByteArray outgoingFrame = QByteArray(payload, payloadLen);
		_frame.remove(0, payloadLen + 4);

		//NonHDLCFrameCoder::_logger.Log(QString("Frame Data Size: %1 (0x%2)").arg(outgoingFrame.count()).arg(outgoingFrame.count(), 4, 16, QChar('0')), false);
		//NonHDLCFrameCoder::_logger.Log(QString("Payload Size: %1 (0x%2)").arg(payloadLen).arg(payloadLen, 4, 16, QChar('0')));
		//NonHDLCFrameCoder::_logger.LogInHex(outgoingFrame, true, 16);
		//NonHDLCFrameCoder::_logger.Log("\n", false);

		_frameFunction(outgoingFrame, _protocolInterface);

		result = true;

		switch (_frame.count())
		{
		case 0:
			break; // do nothing

		case 1:
			break; // do nothing

		default:
			if (_frame.at(0) == kFrameDelimiter && _frame.at(1) == kFrameDelimiter) // There is supposed to be an end frame delimiter, but some implementations omit it
				_frame.remove(0, 1); // If we find one, remove it
		}
	}

	return result;
}

void NonHDLCFrameCoder::HandleBadFrameDataVersion()
{
	int frameDelimiterIndex = _frame.indexOf(kFrameDelimiter, 1); // ignore the first delimiter

	ErrorParameters errors;

	if (frameDelimiterIndex == -1)
	{
		//NonHDLCFrameCoder::_logger.Log("Dumping Bad Frame Data (No Delimiter)");
		//NonHDLCFrameCoder::_logger.LogInHex(_frame, true, 16);
		//NonHDLCFrameCoder::_logger.Log("\n", false);

		_badFrameFunction(_frame, errors, _protocolInterface);
		_frame.clear();  // it's just bad data!
	}
	else
	{
		bool badDataFlag(false);

		QByteArray badData = _frame.left(frameDelimiterIndex);

		if (badData.isEmpty() == false)
		{
			if (badData.size() == 1 && badData.at(0) != kFrameDelimiter)
				badDataFlag = true;
			else
				badDataFlag = true;

			if (badDataFlag == true)
			{
				//NonHDLCFrameCoder::_logger.Log(QString("Dumping Bad Frame Data (Delemiter at:%1)").arg(frameDelimiterIndex));
				//NonHDLCFrameCoder::_logger.LogInHex(badData, true, 16);
				//NonHDLCFrameCoder::_logger.Log("\n", false);

				_badFrameFunction(badData, errors, _protocolInterface);
			}
		}
			
		_frame.remove(0, frameDelimiterIndex);
	}

}
