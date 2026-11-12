// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "PlatformConfigurationException.h"

PlatformConfigurationException::PlatformConfigurationException(std::string const& message) :
    _message(message)
{

}

PlatformConfigurationException::~PlatformConfigurationException()
{

}

std::string PlatformConfigurationException::getMessage() const
{
    return _message;
}
