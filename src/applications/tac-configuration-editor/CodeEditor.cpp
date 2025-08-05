/*
	Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries. 
	 
	Redistribution and use in source and binary forms, with or without
	modification, are permitted (subject to the limitations in the
	disclaimer below) provided that the following conditions are met:
	 
		* Redistributions of source code must retain the above copyright
		  notice, this list of conditions and the following disclaimer.
	 
		* Redistributions in binary form must reproduce the above
		  copyright notice, this list of conditions and the following
		  disclaimer in the documentation and/or other materials provided
		  with the distribution.
	 
		* Neither the name of Qualcomm Technologies, Inc. nor the names of its
		  contributors may be used to endorse or promote products derived
		  from this software without specific prior written permission.
	 
	NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
	GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
	HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
	WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
	ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
	GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
	IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
	IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "CodeEditor.h"
#include "CustomValidator.h"

// QCommon
#include "TableCheckBox.h"
#include "TableComboBox.h"

const int kVariableNameCol(0);
const int kVariableLabelCol(1);
const int kVariableTooltipCol(2);
const int kVariableTypeCol(3);
const int kVariableDefaultValueCol(4);
const int kVariableCellLocationCol(5);
const QString kDefaultCellLocation(QStringLiteral("-1,-1"));
const QByteArray kRow(QByteArrayLiteral("row"));
const QStringList kVariableTypes{"<select type>", "Integer"};
const int kPreviousData(220398);

CodeEditor::CodeEditor
(
	PlatformConfiguration platformConfig,
	QWidget* parent
) :
	QDialog(parent),
	_platformConfiguration(platformConfig)
{
	setupUi(this);

	// Default splitter properties
	QList<int> sizes;
	sizes.append(0.7 * height());
	sizes.append(0.3 * height());
	_editorSplitter->setSizes(sizes);

	setScript();
	readVariables();
	setupVariableTableColumns();
}

CodeEditor::~CodeEditor()
{
}

void CodeEditor::setScript()
{
	_codeEditor->setPlainText(QString(_platformConfiguration->getAlpacaScript()));
}

QByteArray CodeEditor::getScript()
{
	return _codeEditor->toPlainText().toLatin1();
}

void CodeEditor::readVariables()
{
	QTableWidgetItem* twi{Q_NULLPTR};
	TableComboBox* tableComboBox{Q_NULLPTR};
	TableCheckBox* tableCheckBox{Q_NULLPTR};

	for (auto& scriptVariable: _platformConfiguration->getVariables())
	{
		int lastRow = _variableTable->rowCount();
		_variableTable->insertRow(lastRow);

		twi = new QTableWidgetItem;
		twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		twi->setText(scriptVariable._name);
		_variableTable->setItem(lastRow, kVariableNameCol, twi);

		twi = new QTableWidgetItem;
		twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		twi->setText(scriptVariable._label);
		_variableTable->setItem(lastRow, kVariableLabelCol, twi);

		twi = new QTableWidgetItem;
		twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		twi->setText(scriptVariable._tooltip);
		_variableTable->setItem(lastRow, kVariableTooltipCol, twi);

		tableComboBox = new TableComboBox(_variableTable);
		tableComboBox->setupItems(kVariableTypes, kVariableTypes[0]);
		tableComboBox->setCurrentIndex(scriptVariable._type);
		tableComboBox->setProperty(kRow, lastRow);
		connect(tableComboBox, &TableComboBox::currentTextChanged, this, &CodeEditor::onVariableTypeChanged);
		_variableTable->setCellWidget(lastRow, kVariableTypeCol, tableComboBox);

		twi = new QTableWidgetItem;
		twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		QString defaultValue = scriptVariable._defaultValue.toString();
		if (defaultValue.compare("true", Qt::CaseInsensitive) == 0 || defaultValue.compare("false", Qt::CaseInsensitive) == 0)
		{
			tableCheckBox = new TableCheckBox(Q_NULLPTR);
			bool state = (defaultValue == "true")? true : false;
			tableCheckBox->setCheckState(state);
			tableCheckBox->setProperty(kRow, lastRow);
			connect(tableCheckBox, SIGNAL(checkStateChanged(bool)), this, SLOT(onVariableCheckBoxUpdated(bool)));
			_variableTable->setCellWidget(lastRow, kVariableDefaultValueCol, tableCheckBox);
		}
		else
		{
			twi->setText(defaultValue);
			_variableTable->setItem(lastRow, kVariableDefaultValueCol, twi);
		}

		twi = new QTableWidgetItem;
		twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		twi->setText(QString::number(scriptVariable._cellLocation.x()) + "," + QString::number(scriptVariable._cellLocation.y()));
		_variableTable->setItem(lastRow, kVariableCellLocationCol, twi);
	}
	connect(_variableTable, &QTableWidget::itemDoubleClicked, this, &::CodeEditor::onVariableTableItemDoubleClicked);
	connect(_variableTable, &QTableWidget::itemChanged, this, &CodeEditor::onVariableTableItemChanged);
}

void CodeEditor::on_buttonBox_accepted()
{
	_platformConfiguration->setAlpacaScript(QByteArray(_codeEditor->document()->toPlainText().toLatin1()));
}

void CodeEditor::on__addVariable_clicked()
{
	QTableWidgetItem* twi{Q_NULLPTR};
	TableComboBox* tableComboBox{Q_NULLPTR};

	ScriptVariable scriptVar;

	int lastRow = _variableTable->rowCount();
	_variableTable->insertRow(lastRow);

	twi = new QTableWidgetItem;
	twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	twi->setText(scriptVar._name);
	_variableTable->setItem(lastRow, kVariableNameCol, twi);

	twi = new QTableWidgetItem;
	twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	twi->setText(scriptVar._label);
	_variableTable->setItem(lastRow, kVariableLabelCol, twi);

	twi = new QTableWidgetItem;
	twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	twi->setText(scriptVar._tooltip);
	_variableTable->setItem(lastRow, kVariableTooltipCol, twi);

	tableComboBox = new TableComboBox(Q_NULLPTR);
	tableComboBox->setupItems(kVariableTypes, kVariableTypes[0]);
	tableComboBox->setCurrentIndex(scriptVar._type);
	tableComboBox->setProperty(kRow, lastRow);
	connect(tableComboBox, &TableComboBox::currentTextChanged, this, &CodeEditor::onVariableTypeChanged);
	_variableTable->setCellWidget(lastRow, kVariableTypeCol, tableComboBox);

	twi = new QTableWidgetItem;
	twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	twi->setText(scriptVar._defaultValue.toString());
	_variableTable->setItem(lastRow, kVariableDefaultValueCol, twi);

	twi = new QTableWidgetItem;
	twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	twi->setText(QString::number(scriptVar._cellLocation.x()) + "," + QString::number(scriptVar._cellLocation.y()));
	_variableTable->setItem(lastRow, kVariableCellLocationCol, twi);
}


void CodeEditor::on__removeVariable_clicked()
{
	QList<QModelIndex> rowList = _variableTable->selectionModel()->selectedRows();
	for (auto twi : rowList)
	{
		QString variableName = _variableTable->itemAt(kVariableNameCol, twi.row())->text();
	   _variableTable->removeRow(twi.row());
		_platformConfiguration->deleteVariable(variableName);
	}

	int varCount = _variableTable->rowCount();
	if (varCount == 0)
	   _removeVariable->setEnabled(false);
}

void CodeEditor::setupVariableTableColumns()
{
	_variableTable->setFocusPolicy(Qt::NoFocus);

	// No spaces in the pin command string
	_variableTable->setItemDelegateForColumn(kVariableNameCol, new CustomValidator(_variableTable, eNoSpaceValidator));

	// No semicolons allowed in label and tooltip columns
	_variableTable->setItemDelegateForColumn(kVariableLabelCol, new CustomValidator(_variableTable, eNoSemiColonValidator));
	_variableTable->setItemDelegateForColumn(kVariableTooltipCol, new CustomValidator(_variableTable, eNoSemiColonValidator));

	// Limit length of pin label string
	_variableTable->setItemDelegateForColumn(kVariableDefaultValueCol, new CustomValidator(_variableTable, eNumericOnlyValidator));

	// Limit spaces in the pin command string
	_variableTable->setItemDelegateForColumn(kVariableCellLocationCol, new CustomValidator(_variableTable, eCellLocationValidator));

	QHeaderView* headerView = _variableTable->horizontalHeader();

	_variableTable->resizeColumnToContents(kVariableNameCol);
	headerView->setSectionResizeMode(kVariableNameCol, QHeaderView::Stretch);

	_variableTable->resizeColumnToContents(kVariableLabelCol);
	headerView->setSectionResizeMode(kVariableLabelCol, QHeaderView::Stretch);

	_variableTable->resizeColumnToContents(kVariableTooltipCol);
	headerView->setSectionResizeMode(kVariableTooltipCol, QHeaderView::Stretch);

	_variableTable->setColumnWidth(kVariableTypeCol, 100);
	headerView->setSectionResizeMode(kVariableTypeCol, QHeaderView::Fixed);

	_variableTable->setColumnWidth(kVariableDefaultValueCol, 130);
	headerView->setSectionResizeMode(kVariableDefaultValueCol, QHeaderView::Fixed);

	_variableTable->setColumnWidth(kVariableCellLocationCol, 100);
	headerView->setSectionResizeMode(kVariableCellLocationCol, QHeaderView::Fixed);
}

void CodeEditor::on__variableTable_cellClicked(int row, int column)
{
	if (row >=0 && column >=0)
	{
	   _removeVariable->setEnabled(true);
	}
}

void CodeEditor::onVariableTypeChanged(QString newType)
{
	QTableWidgetItem* twi{Q_NULLPTR};
	TableComboBox* tComboBox{Q_NULLPTR};
	TableCheckBox* tCheckBox{Q_NULLPTR};

	tComboBox = qobject_cast<TableComboBox*>(sender());
	if (tComboBox != Q_NULLPTR)
	{
		unsigned int row = tComboBox->property(kRow).toUInt();

		QString variableName = _variableTable->item(row, kVariableNameCol)->text();
		VariableType scriptVariableType = variableTypeFromString(newType);
		_platformConfiguration->setVariableType(variableName, scriptVariableType);

		if (scriptVariableType == VariableType::eUnknownVariableType)
		{
			twi = new QTableWidgetItem(Q_NULLPTR);
			_variableTable->removeCellWidget(row, kVariableDefaultValueCol);

			twi->setText("<no value>");
			twi->setFlags(twi->flags() & ~Qt::ItemIsEditable);
			_variableTable->setItem(row, kVariableDefaultValueCol, twi);
		}
		else if (scriptVariableType == VariableType::eBooleanType)
		{
			// Set the variable state in the UI to false always when checkbox is selected
			// Additionally, set it state in the platform configuration because the state will only update on next checkbox click.
			bool state{false};

			tCheckBox = new TableCheckBox(Q_NULLPTR);
			tCheckBox->setCheckState(state);
			_platformConfiguration->setVariableDefaultValue(variableName, state);
			tCheckBox->setProperty(kRow, row);

			if (tCheckBox != Q_NULLPTR)
			{
				_variableTable->item(row, kVariableDefaultValueCol)->setText("");
				connect(tCheckBox, SIGNAL(checkStateChanged(bool)), this, SLOT(onVariableCheckBoxUpdated(bool)));
				_variableTable->setCellWidget(row, kVariableDefaultValueCol, tCheckBox);
			}
		}
		else
		{
			ScriptVariables scriptVariables = _platformConfiguration->getVariables();
			twi = new QTableWidgetItem(Q_NULLPTR);

			if (twi != Q_NULLPTR)
			{
				twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				if (scriptVariables.contains(variableName))
				{
					QString variableValue = scriptVariables[variableName]._defaultValue.toString();
					twi->setText(variableValue);
				}
				else
				{
					twi->setText("<no value>");
				}

				_variableTable->removeCellWidget(row, kVariableDefaultValueCol);
				_variableTable->setItem(row, kVariableDefaultValueCol, twi);
			}
		}
	}
}

void CodeEditor::onVariableTableItemChanged(QTableWidgetItem *twi)
{
	QString currVariableName, prevVariableName;

	QTableWidgetItem* vtwi = _variableTable->item(twi->row(), kVariableNameCol);
	if (vtwi)
	{
		currVariableName = vtwi->text();
		prevVariableName = vtwi->data(kPreviousData).toString();
	}

	QString currentItemText = twi->text();

	if (_platformConfiguration != Q_NULLPTR)
	{
		switch(twi->column())
		{
		case kVariableNameCol:
		   _platformConfiguration->setVariableName(currVariableName);
		   break;
		case kVariableLabelCol:
		   _platformConfiguration->setVariableLabel(currVariableName, currentItemText);
		   break;
		case kVariableTooltipCol:
		   _platformConfiguration->setVariableTooltip(currVariableName, currentItemText);
		   break;
		case kVariableDefaultValueCol:
		   _platformConfiguration->setVariableDefaultValue(currVariableName, currentItemText);
		   break;
		case kVariableCellLocationCol:
		{
		   QStringList rowColString = currentItemText.split(",");
		   if (rowColString.size() == 2)
		   {
			   qint32 row = rowColString.at(0).toInt();
			   qint32 col = rowColString.at(1).toInt();

			   if (row < 0 || col < 0)
			   {
				   twi->setText(kDefaultCellLocation);
				   _platformConfiguration->setVariableCellLocation(currVariableName, QPoint(-1,-1));
			   }
			   else
				   _platformConfiguration->setVariableCellLocation(currVariableName, QPoint(row, col));
		   }
		   else
		   {
			   twi->setText(kDefaultCellLocation);
			   _platformConfiguration->setVariableCellLocation(currVariableName, QPoint(-1,-1));
		   }
		   break;
		}
		default:
		   break;
		}

		if (twi->column() == kVariableNameCol && currVariableName.compare(prevVariableName) != 0)
		{
			ScriptVariables scriptVariables = _platformConfiguration->getVariables();

			_platformConfiguration->setVariableLabel(currVariableName, scriptVariables[prevVariableName]._label);
			_platformConfiguration->setVariableTooltip(currVariableName, scriptVariables[prevVariableName]._tooltip);
			_platformConfiguration->setVariableType(currVariableName, scriptVariables[prevVariableName]._type);
			_platformConfiguration->setVariableDefaultValue(currVariableName, scriptVariables[prevVariableName]._defaultValue);
			_platformConfiguration->setVariableCellLocation(currVariableName, scriptVariables[prevVariableName]._cellLocation);

			_platformConfiguration->deleteVariable(prevVariableName);
		}

		QString lastError = _platformConfiguration->getLastError();

		if (lastError.isEmpty() == false)
			emit startNotification(lastError);
	}
}

void CodeEditor::onVariableCheckBoxUpdated(bool newState)
{
	TableCheckBox* tCheckBox = qobject_cast<TableCheckBox*>(sender());
	if (tCheckBox != Q_NULLPTR)
	{
	   quint32 row = tCheckBox->property(kRow).toInt();
	   QString variableName = _variableTable->item(row, kVariableNameCol)->text();
	   _platformConfiguration->setVariableDefaultValue(variableName, newState);
	}
}

void CodeEditor::onVariableTableItemDoubleClicked(QTableWidgetItem *twi)
{
	if (twi && twi->column() == kVariableNameCol)
		twi->setData(kPreviousData, twi->text());
}

void CodeEditor::on__variableTable_itemClicked(QTableWidgetItem *item)
{
	Q_UNUSED(item);
	emit ScriptVariablesUpdated();
}

