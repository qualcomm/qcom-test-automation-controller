// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "PlatformConfigurationException.h"

PlatformConfigurationException::PlatformConfigurationException(const std::string& message) :
	QTACException(message)
{
}
