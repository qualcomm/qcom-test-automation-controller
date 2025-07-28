#ifndef COUNTINGSTREAM_H
#define COUNTINGSTREAM_H
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
// Copyright 2020 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "QCommonConsoleGlobal.h"

#include <QtGlobal>

class QCOMMONCONSOLE_EXPORT CountingStream
{
public:
	CountingStream();
	~CountingStream();

	quint64 count()
	{
		return _count;
	}
	void resetStatus()
	{
		_count = 0;
	}

	CountingStream &operator<<(qint8 i);
	CountingStream &operator<<(quint8 i);
	CountingStream &operator<<(qint16 i);
	CountingStream &operator<<(quint16 i);
	CountingStream &operator<<(qint32 i);
	inline CountingStream &operator<<(quint32 i);
	CountingStream &operator<<(qint64 i);
	CountingStream &operator<<(quint64 i);
	CountingStream &operator<<(bool i);
	CountingStream &operator<<(float f);
	CountingStream &operator<<(double f);
	CountingStream &operator<<(const char *str);

	CountingStream &writeBytes(const char *, uint len);
    int writeRawData(const char *, int len);

private:
	quint64						_count = 0;
};

/*****************************************************************************
  CountingStream inline functions
 *****************************************************************************/

inline CountingStream& CountingStream::operator<<(quint8 i)
{ return *this << qint8(i); }

inline CountingStream& CountingStream::operator<<(quint16 i)
{ return *this << qint16(i); }

inline CountingStream& CountingStream::operator<<(quint32 i)
{ return *this << qint32(i); }

inline CountingStream& CountingStream::operator<<(quint64 i)
{ return *this << qint64(i); }

#endif // COUNTINGSTREAM_H
