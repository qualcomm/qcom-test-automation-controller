#ifndef COUNTINGSTREAM_H
#define COUNTINGSTREAM_H
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
