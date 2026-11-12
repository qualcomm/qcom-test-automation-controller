#ifndef BASICCOLOR_H
#define BASICCOLOR_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// QT's color class is based in the gui lib.  We want this library to not be dependant on gui or widgets.

#include <cstdint>
#include <string>

const uint16_t kOpaque{255};

class BasicColor
{

public:
	BasicColor() = default;
	BasicColor(uint16_t red, uint16_t green, uint16_t blue, uint16_t alpha = kOpaque);
	BasicColor(const BasicColor& copyMe) = default;
	BasicColor& operator= (const BasicColor& assignMe) = default;

	bool isValid()
	{
		return _valid;
	}

	uint16_t red() const { return _red; }
	uint16_t green() const { return _green; }
	uint16_t blue() const { return _blue; }
	uint16_t alpha() const { return _alpha; }

	std::string name() const;
	void setNamedColor(const std::string& namedColor);

	BasicColor lighter(uint16_t scale);

private:
	bool						_valid{false};
	uint16_t					_alpha{kOpaque};
	uint16_t					_red{0};
	uint16_t					_green{0};
	uint16_t					_blue{0};
};

#endif // BASICCOLOR_H
