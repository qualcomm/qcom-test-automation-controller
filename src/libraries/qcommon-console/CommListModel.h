#ifndef COMLISTMODEL_H
#define COMLISTMODEL_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: msimpson
*/

//Qt
#include <QtGui/QStandardItemModel>

class  CommListModel :
	public QStandardItemModel
{
Q_OBJECT

public:
	CommListModel(QObject* parent = NULL);

	void Refresh();

signals:
	void ModelUpdated();
};

#endif // COMLISTMODEL_H

