// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef PSOCSERIALTABLEMODEL_H
#define PSOCSERIALTABLEMODEL_H

#include "QCommonConsoleGlobal.h"

#include "SerialTableModel.h"

const int kPortNameCol(0);
const int kDescriptionCol(1);
const int kSerialNumberCol(2);

class QCOMMONCONSOLE_EXPORT PSOCSerialTableModel :
	public SerialTableModel
{
public:
	PSOCSerialTableModel(QObject* parent = Q_NULLPTR);

	QVariant portData(int deviceIndex, int dataIndex);
};

#endif // PSOCSERIALTABLEMODEL_H
