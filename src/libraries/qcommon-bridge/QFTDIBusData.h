#ifndef QFTDIBUSDATA_H
#define QFTDIBUSDATA_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonBridgeGlobal.h"
#include "QCommonTypes.h"
#include "FTDIPinData.h"

// Qt
#include <QString>

namespace QFTDIBusData
{
	inline QString toString(FTDIBusFunction busFunction)
	{
		return toQString(FTDIBusData::toString(busFunction));
	}

	inline FTDIBusFunction fromString(const QString& busFunctionStr)
	{
		return FTDIBusData::fromString(toStdString(busFunctionStr));
	}
}

#endif // QFTDIBUSDATA_H
