// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	%Author: msimpson, biswroy %
*/

#include "CommListModel.h"

// Qt
#include <QtCore/QSize>
#include <QtGui/QColor>

CommListModel::CommListModel(QObject* parent) :
	QStandardItemModel(parent)
{
	QStringList headers;

	headers << tr("Port Number") << tr("Port Description") ;

	setHorizontalHeaderLabels(headers);
}

void CommListModel::Refresh()
{
	removeRows(0, rowCount());
}
