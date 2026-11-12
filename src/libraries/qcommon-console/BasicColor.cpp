// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "BasicColor.h"

#include <cstdio>
#include <cstdlib>

static std::string toHexByte(uint16_t value)
{
	char buffer[3];

	std::snprintf(buffer, sizeof(buffer), "%02x", value);

	return std::string(buffer);
}

BasicColor::BasicColor
(
	uint16_t red,
	uint16_t green,
	uint16_t blue,
	uint16_t alpha
)
{
	_valid = true;
	_red = red;
	_green = green;
	_blue = blue;
	_alpha = alpha;
}

std::string BasicColor::name() const
{
	std::string result("#");

	result += toHexByte(_red);
	result += toHexByte(_green);
	result += toHexByte(_blue);

	return  result;
}

void BasicColor::setNamedColor
(
	const std::string& namedColor
)
{
	_valid = false;

	if (namedColor[0] == '#')
	{
		if (namedColor.length() >= 7)
		{
			std::string redString = namedColor.substr(1, 2);
			std::string greenString = namedColor.substr(3, 2);
			std::string blueString = namedColor.substr(5, 2);

			_red = static_cast<uint16_t>(std::strtoul(redString.c_str(), nullptr, 16));
			_green = static_cast<uint16_t>(std::strtoul(greenString.c_str(), nullptr, 16));
			_blue = static_cast<uint16_t>(std::strtoul(blueString.c_str(), nullptr, 16));

			_valid = true;
		}
	}
}

BasicColor BasicColor::lighter(uint16_t scale)
{
	BasicColor result(_red, _green, _blue);

	if (scale > 100 && scale < 151)
	{
		double factor = scale / 100.0;
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
