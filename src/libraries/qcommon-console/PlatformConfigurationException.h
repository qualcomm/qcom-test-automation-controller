#ifndef PLATFORMCONFIGURATIONEXCEPTION_H
#define PLATFORMCONFIGURATIONEXCEPTION_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QTACException.h"

#include <string>

class QCOMMONCONSOLE_EXPORT PlatformConfigurationException :
	public QTACException
{
public:
	explicit PlatformConfigurationException(const std::string& message);
	~PlatformConfigurationException() override = default;

	void raise() const override
	{
		throw *this;
	}

	PlatformConfigurationException* clone() const override
	{
		return new PlatformConfigurationException(*this);
	}
};

#endif // PLATFORMCONFIGURATIONEXCEPTION_H
