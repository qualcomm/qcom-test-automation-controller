#ifndef SCRIPTVARIABLE_H
#define SCRIPTVARIABLE_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "QCommonConsoleGlobal.h"
#include "StringUtilities.h"

// Qt
#include <QList>
#include <QPoint>
#include <QString>
#include <QVariant>

const QString kDefaultVariableName(QStringLiteral("<variable name>"));
const QString kDefaultVariableLabel(QStringLiteral("<label for the variable>"));
const QString kDefaultVariableTooltip(QStringLiteral("<tooltip for the variable>"));


enum VariableType
{
	eUnknownVariableType = 0,
	eIntegerType,
    eBooleanType,
	eFloatType
};

struct QCOMMONCONSOLE_EXPORT ScriptVariable
{
public:
	ScriptVariable() = default;
	~ScriptVariable() = default;
	ScriptVariable(const ScriptVariable& copyMe) = default;

	QString                     _name{kDefaultVariableName};
	QString						_label{kDefaultVariableLabel};
	QString						_tooltip{kDefaultVariableTooltip};
	VariableType				_type{eUnknownVariableType};
	QVariant                	_defaultValue{0};
	QPoint						_cellLocation{QPoint(-1,-1)};
};

typedef QMap<QString,ScriptVariable> ScriptVariables;

QString QCOMMONCONSOLE_EXPORT variableTypeToString(VariableType variableType);
VariableType QCOMMONCONSOLE_EXPORT variableTypeFromString(const QString& typeString);

#endif // SCRIPTVARIABLE_H
