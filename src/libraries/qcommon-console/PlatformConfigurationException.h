#ifndef PLATFORMCONFIGURATIONEXCEPTION_H
#define PLATFORMCONFIGURATIONEXCEPTION_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
    Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "QCommonConsoleGlobal.h"

#include <QException>

/**
 * @brief The PlatformConfigurationException class - Custom exception class to throw exceptions on invalid API usage.
 */
class QCOMMONCONSOLE_EXPORT PlatformConfigurationException :
	public QException
{
public:
    PlatformConfigurationException(QString const& message);
    ~PlatformConfigurationException();

    void raise() const
    {
        throw *this;
    }

    PlatformConfigurationException* clone() const
    {
        return new PlatformConfigurationException(*this);
    }

    QString getMessage() const;

private:
    QString                 _message;
};

#endif // PLATFORMCONFIGURATIONEXCEPTION_H
