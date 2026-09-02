// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "PSOCI2CDialog.h"

// Qt
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QSet>
#include <QSpacerItem>
#include <QSpinBox>

const int kMaxI2CSlaves(3);
const int kMinI2CSlaves(1);

const QByteArray kDefaultAddSlave(QByteArrayLiteral("Click to add I2C Slave"));
const QByteArray kDefaultRemoveSlave(QByteArrayLiteral("Click to remove the last I2C Slave"));
const QByteArray kCannotAddSlave(QByteArrayLiteral("Cannot add more than 3 slaves."));
const QByteArray kCannotRemoveSlave(QByteArrayLiteral("At least 1 slave is needed for IO Expander. Else, choose GPIO only."));
const QByteArray kDuplicateAddress(QByteArrayLiteral("Slave addresses must be unique."));
const QByteArray kDefaultOkTooltip(QByteArrayLiteral(""));
const QStringList kChipNames{"<select slave>", "KTS1622EUAATR", "TCA9534APWR"};

PSOCI2CDialog::PSOCI2CDialog(_PSOCPlatformConfiguration* psocConfig, QWidget *parent) : QDialog(parent)
{
	setupUi(this);

	_psocConfig = psocConfig;

	auto slaves = _psocConfig->getSlaveConfigs();

	for (int row = 0; row < slaves.count(); ++row)
		_slaveRows.append(addSlaveRow(row, slaves.at(row)));

	updateButtonStates();
	validateAddresses();
}

PSOCI2CDialog::~PSOCI2CDialog()
{
}

void PSOCI2CDialog::on__addSlaveBtn_clicked()
{
	auto slaves = _psocConfig->getSlaveConfigs();

	if (slaves.count() < kMaxI2CSlaves)
	{
		PSOCI2CSlave slave;

		slaves.append(slave);
		_psocConfig->setSlaveConfigs(slaves);

		_slaveRows.append(addSlaveRow(_slaveRows.count(), slave));
	}

	updateButtonStates();
	validateAddresses();
}

void PSOCI2CDialog::on__removeSlaveBtn_clicked()
{
	auto slaves = _psocConfig->getSlaveConfigs();

	if (slaves.count() > kMinI2CSlaves)
	{
		if (!_slaveRows.isEmpty())
		{
			QWidget* wgt = _slaveRows.takeLast();
			if (wgt != Q_NULLPTR)
			{
				wgt->hide();
				wgt->deleteLater();

				slaves.removeLast();
				_psocConfig->setSlaveConfigs(slaves);
			}
		}
	}

	updateButtonStates();
	validateAddresses();
}

void PSOCI2CDialog::updateButtonStates()
{
	auto slaves = _psocConfig->getSlaveConfigs();
	int sc = slaves.count();

	if (sc == kMinI2CSlaves)
	{
		_removeSlaveBtn->setEnabled(false);
		_removeSlaveBtn->setToolTip(kCannotRemoveSlave);
	}
	else if (sc == kMaxI2CSlaves)
	{
		_addSlaveBtn->setEnabled(false);
		_addSlaveBtn->setToolTip(kCannotAddSlave);
	}
	else
	{
		_addSlaveBtn->setEnabled(true);
		_addSlaveBtn->setToolTip(kDefaultAddSlave);

		_removeSlaveBtn->setEnabled(true);
		_removeSlaveBtn->setToolTip(kDefaultRemoveSlave);
	}
}

void PSOCI2CDialog::validateAddresses()
{
	auto slaves = _psocConfig->getSlaveConfigs();

	QSet<PinID> seenAddresses;
	bool duplicate(false);

	for (const auto& slave : std::as_const(slaves))
	{
		if (seenAddresses.contains(slave._slaveAddress))
		{
			duplicate = true;
			break;
		}
		seenAddresses.insert(slave._slaveAddress);
	}

	_okBtn->setEnabled(!duplicate);
	_okBtn->setToolTip(duplicate ? kDuplicateAddress : kDefaultOkTooltip);
}

void PSOCI2CDialog::updateSlaveFromRow(int row)
{
	QWidget* rowWgt = _slaveRows.at(row);

	QComboBox* cb = rowWgt->findChild<QComboBox*>();
	QList<QLineEdit*> lineEdits = rowWgt->findChildren<QLineEdit*>();
	QSpinBox* sb = rowWgt->findChild<QSpinBox*>();

	if (cb == Q_NULLPTR || lineEdits.count() < 2 || sb == Q_NULLPTR)
		return;

	auto slaves = _psocConfig->getSlaveConfigs();

	if (row >= slaves.count())
		return;

	PSOCI2CSlave slave = slaves.at(row);

	slave._variant = static_cast<PSOCIICVariant>(cb->currentIndex());

	bool ok(false);
	PinID slaveAddress = static_cast<PinID>(lineEdits.at(0)->text().toUInt(&ok, 16));
	if (ok)
		slave._slaveAddress = slaveAddress;

	PinID configAddress = static_cast<PinID>(lineEdits.at(1)->text().toUInt(&ok, 16));
	if (ok)
		slave._configAddress = configAddress;

	slave._portCount = sb->value();

	slaves[row] = slave;
	_psocConfig->setSlaveConfigs(slaves);

	validateAddresses();
}

QWidget* PSOCI2CDialog::addSlaveRow(int row, const PSOCI2CSlave &slave)
{
	QComboBox* cb{Q_NULLPTR};
	QLabel* l{Q_NULLPTR};
	QLineEdit* slaveAddrEdit{Q_NULLPTR};
	QLineEdit* configAddrEdit{Q_NULLPTR};
	QSpinBox* sb{Q_NULLPTR};

	QWidget* swgt = new QWidget(_slaveGroupBox);
	QHBoxLayout* hbl = new QHBoxLayout(swgt);
	hbl->setContentsMargins(1, 1, 1, 1);

	l = new QLabel(swgt);
	l->setText("Chip Name:");
	hbl->addWidget(l);

	cb = new QComboBox(swgt);
	cb->addItems(kChipNames);
	cb->setCurrentIndex(slave._variant);
	hbl->addWidget(cb);

	l = new QLabel(swgt);
	l->setText("Slave Address:");
	hbl->addWidget(l);

	slaveAddrEdit = new QLineEdit(swgt);
	slaveAddrEdit->setText(QString::number(slave._slaveAddress, 16));
	hbl->addWidget(slaveAddrEdit);

	l = new QLabel(swgt);
	l->setText("Config Address:");
	hbl->addWidget(l);

	configAddrEdit = new QLineEdit(swgt);
	configAddrEdit->setText(QString::number(slave._configAddress, 16));
	hbl->addWidget(configAddrEdit);

	l = new QLabel(swgt);
	l->setText("Port Count:");
	hbl->addWidget(l);

	sb = new QSpinBox(swgt);
	sb->setMinimum(1);
	sb->setMaximum(3);
	sb->setValue(slave._portCount == 0 ? 1 : slave._portCount);
	hbl->addWidget(sb);

	connect(cb, &QComboBox::currentIndexChanged, this, [this, row]() { updateSlaveFromRow(row); });
	connect(slaveAddrEdit, &QLineEdit::editingFinished, this, [this, row]() { updateSlaveFromRow(row); });
	connect(configAddrEdit, &QLineEdit::editingFinished, this, [this, row]() { updateSlaveFromRow(row); });
	connect(sb, &QSpinBox::editingFinished, this, [this, row]() { updateSlaveFromRow(row); });

	// this is the group-box layout
	verticalLayout->addWidget(swgt);

	return swgt;
}
