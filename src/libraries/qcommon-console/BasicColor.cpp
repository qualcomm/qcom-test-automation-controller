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

#include "BasicColor.h"

BasicColor::BasicColor
(
	quint16 red,
	quint16 green,
	quint16 blue,
	quint16 alpha
)
{
	_valid = true;
	_red = red;
	_green = green;
	_blue = blue;
	_alpha = alpha;
}

QString BasicColor::name() const
{
	QString result("#");

    result +=  QString("%1").arg(_red, 2, 16, QLatin1Char('0'));
    result +=  QString("%1").arg(_green, 2, 16, QLatin1Char('0'));
    result +=  QString("%1").arg(_blue, 2, 16, QLatin1Char('0'));

	return  result;
}

void BasicColor::setNamedColor
(
	const QString& namedColor
)
{
	_valid = false;

	if (namedColor[0] == '#')
	{
		if (namedColor.length() >= 7)
		{
			QString redString = namedColor.mid(1, 2);
			QString greenString = namedColor.mid(3, 2);
			QString blueString = namedColor.mid(5, 2);

			_red = redString.toUShort(Q_NULLPTR, 16);
			_green = greenString.toUShort(Q_NULLPTR, 16);
			_blue = blueString.toUShort(Q_NULLPTR, 16);

			_valid = true;
		}
	}
}

BasicColor BasicColor::lighter(quint16 scale)
{
	BasicColor result(_red, _green, _blue);

	if (scale > 100 && scale < 151)
	{
		qreal factor = scale / 100.0;
		if (factor)
		{
			result._red = _red * factor;
			if (result.red() > 255)
				result._red = 255;

			result._green = _green * factor;
			if (result.green() > 255)
				result._green = 255;

			result._blue = _blue * factor;
			if (result.blue() > 255)
				result._blue = 255;

			result._valid = true;
		}
	}

	return result;
}
