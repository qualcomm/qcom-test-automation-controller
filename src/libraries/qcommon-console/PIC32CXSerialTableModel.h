// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef PIC32CXSERIALTABLEMODEL_H
#define PIC32CXSERIALTABLEMODEL_H


#include "QCommonConsoleGlobal.h"

#include "SerialTableModel.h"

const int kPortNameCol(0);
const int kDescriptionCol(1);
const int kSerialNumberCol(2);

class QCOMMONCONSOLE_EXPORT PIC32CXSerialTableModel :
	public SerialTableModel
{
public:
	PIC32CXSerialTableModel(QObject* parent = Q_NULLPTR);

	QVariant portData(int deviceIndex, int dataIndex);
};

#endif // PIC32CXSERIALTABLEMODEL_H
