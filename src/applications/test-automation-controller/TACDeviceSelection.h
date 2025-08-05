#ifndef TACDEVICESELECTION_H
#define TACDEVICESELECTION_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

// Author: Michael Simpson msimpson@qti.qualcomm.com

#include "ui_TACDeviceSelection.h"

// QT
#include <QDialog>
#include <QMap>


class QTimer;

class TACDeviceSelection :
	public QDialog,
	public Ui::TACDeviceSelectionClass
{
Q_OBJECT

public:
	TACDeviceSelection(QWidget* parent);
	~TACDeviceSelection();

	QByteArray getSelectedPortName();

public slots:
	void on__tacDevices_clicked(const QModelIndex& index);
	void on__tacDevices_doubleClicked(const QModelIndex& index);
	void timer_timeout();

private:
	void enableOk(bool state = true);

	QByteArray					_selectedPortName;
	QTimer*						_timer{Q_NULLPTR};
};

#endif // TACDEVICESELECTION_H
