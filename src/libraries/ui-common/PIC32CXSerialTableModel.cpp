// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "PIC32CXSerialTableModel.h"

PIC32CXSerialTableModel::PIC32CXSerialTableModel(QObject* parent) :
	SerialTableModel(parent)
{
	addFilterKey(0x04D8, 0x000A);
}

QVariant PIC32CXSerialTableModel::portData(int deviceIndex, int dataIndex)
{
	return data(createIndex(deviceIndex,  dataIndex));
}
