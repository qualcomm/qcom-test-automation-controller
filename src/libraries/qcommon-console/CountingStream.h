#ifndef COUNTINGSTREAM_H
#define COUNTINGSTREAM_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

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
