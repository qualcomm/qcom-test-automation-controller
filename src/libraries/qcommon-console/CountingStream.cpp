// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "CountingStream.h"

// QT
#include <QByteArray>

CountingStream::CountingStream()
{
}

CountingStream::~CountingStream()
{
}

CountingStream& CountingStream::operator<<(qint8)
{
   _count += sizeof(qint8);
    return *this;
}

CountingStream& CountingStream::operator<<(qint16)
{
	_count += sizeof(qint16);
	 return *this;
}

CountingStream& CountingStream::operator<<(qint32)
{
	_count += sizeof(qint32);
	 return *this;
}

CountingStream& CountingStream::operator<<(qint64)
{
	_count += sizeof(qint64);
	 return *this;
}

CountingStream& CountingStream::operator<<(bool)
{
	_count += sizeof(bool);
	 return *this;
}

CountingStream& CountingStream::operator<<(float)
{
	_count += sizeof(float);
    return *this;
}

CountingStream& CountingStream::operator<<(double)
{
	_count += sizeof(double);
	return *this;
}

CountingStream& CountingStream::operator<<(const char *s)
{
	_count += qstrlen(s) + 1;
    return *this;
}

CountingStream& CountingStream::writeBytes(const char*, uint len)
{
	_count += len;
	return *this;
}

int CountingStream::writeRawData(const char*, int len)
{
	_count += len;
	return len;
}

