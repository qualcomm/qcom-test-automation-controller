// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "LicenseFailEvent.h"

LicenseFailEvent::LicenseFailEvent
(
	const QString &why
) :
	QEvent(kLicenseFailEvent),
	_why(why)
{

}
