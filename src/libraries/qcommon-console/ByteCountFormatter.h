#ifndef BYTECOUNTFORMATTER_H
#define BYTECOUNTFORMATTER_H

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

#include <QtGlobal>
#include <QString>

inline QString FormatBytes
(
	quint64 byteCount,
	int precision = -1
)
{
	double	divisor;

	const quint64 kKiloByte(1024);
	const quint64 kMegaByte(kKiloByte * 1024);
	const quint64 kGigaByte(kMegaByte * 1024);
	const quint64 kTeraByte(kGigaByte * 1024);

	int unitArrayIndex;
	QString unitArray[] =
	{
		QString("B"),
		QString("KB"),
		QString("MB"),
		QString("GB"),
		QString("TB")
	};

	if (byteCount < kKiloByte)
	{
		unitArrayIndex = 0;
		divisor	=	0;
	}
	else if (byteCount < kMegaByte)
	{
		unitArrayIndex = 1;
		divisor	= kKiloByte;
	}
	else if (byteCount < kGigaByte)
	{
		unitArrayIndex = 2;
		divisor = kMegaByte;
	}
	else if (byteCount < kTeraByte)
	{
		unitArrayIndex = 3;
		divisor	= kGigaByte;
	}
	else
	{
		unitArrayIndex = 4;
		divisor = kTeraByte;
	}

	if	(divisor ==	0.0)
		return QString("%1 %2").arg((double) byteCount,	0, 'f', precision).arg(unitArray[unitArrayIndex]);

	return QString("%1 %2").arg((double) byteCount / divisor, 0, 'f', precision).arg(unitArray[unitArrayIndex]);
}

#endif // BYTECOUNTFORMATTER_H
