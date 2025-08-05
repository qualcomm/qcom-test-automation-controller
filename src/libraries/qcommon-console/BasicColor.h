#ifndef BASICCOLOR_H
#define BASICCOLOR_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// QT's color class is based in the gui lib.  We want this library to not be dependant on gui or widgets.

#include <QtGlobal>

const quint16 kOpaque{255};

#include <QString>

class BasicColor
{

public:
	BasicColor() = default;
	BasicColor(quint16 red, quint16 green, quint16 blue, quint16 alpha = kOpaque);
	BasicColor(const BasicColor& copyMe) = default;
	BasicColor& operator= (const BasicColor& assignMe) = default;

	bool isValid()
	{
		return _valid;
	}

	quint16 red() const { return _red; }
	quint16 green() const { return _green; }
	quint16 blue() const { return _blue; }
	quint16 alpha() const { return _alpha; }

	QString name() const;
	void setNamedColor(const QString& namedColor);

	BasicColor lighter(quint16 scale);

private:
	bool						_valid{false};
	quint16						_alpha{kOpaque};
	quint16						_red{0};
	quint16						_green{0};
	quint16						_blue{0};
};

#endif // BASICCOLOR_H
