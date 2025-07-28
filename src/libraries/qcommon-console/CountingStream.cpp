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

