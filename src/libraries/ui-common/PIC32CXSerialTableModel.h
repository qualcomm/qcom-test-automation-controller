// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef PIC32CXSERIALTABLEMODEL_H
#define PIC32CXSERIALTABLEMODEL_H

#include "UIGlobalLib.h"
#include "SerialTableModel.h"

const int kPortNameCol(0);
const int kDescriptionCol(1);
const int kSerialNumberCol(2);

class UILIB_EXPORT PIC32CXSerialTableModel :
	public SerialTableModel
{
public:
	PIC32CXSerialTableModel(QObject* parent = nullptr);

	QVariant portData(int deviceIndex, int dataIndex);
};

#endif // PIC32CXSERIALTABLEMODEL_H
