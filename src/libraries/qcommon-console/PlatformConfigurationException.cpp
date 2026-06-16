// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "PlatformConfigurationException.h"

PlatformConfigurationException::PlatformConfigurationException(QString const& message) :
    _message(message)
{

}

PlatformConfigurationException::~PlatformConfigurationException()
{

}

QString PlatformConfigurationException::getMessage() const
{
    return _message;
}
