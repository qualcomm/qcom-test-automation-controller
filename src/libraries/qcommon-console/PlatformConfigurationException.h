#ifndef PLATFORMCONFIGURATIONEXCEPTION_H
#define PLATFORMCONFIGURATIONEXCEPTION_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"

#include <QException>

#include <string>

class QCOMMONCONSOLE_EXPORT PlatformConfigurationException :
	public QException
{
public:
    PlatformConfigurationException(std::string const& message);
    ~PlatformConfigurationException();

    void raise() const
    {
        throw *this;
    }

    PlatformConfigurationException* clone() const
    {
        return new PlatformConfigurationException(*this);
    }

    std::string getMessage() const;

private:
    std::string              _message;
};

#endif // PLATFORMCONFIGURATIONEXCEPTION_H
