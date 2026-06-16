// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "PSOCSerialTableModel.h"

PSOCSerialTableModel::PSOCSerialTableModel
(
	QObject* parent
) :
	SerialTableModel(parent)
{
	addFilterKey(0x05C6, 0x9302);
	addFilterKey(0x16C0, 0x0483);
}

QVariant PSOCSerialTableModel::portData
(
	int deviceIndex,
	int dataIndex
)
{
	return data(createIndex(deviceIndex,  dataIndex));
}
