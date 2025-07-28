#ifndef SCRIPTVARIABLE_H
#define SCRIPTVARIABLE_H

// Confidential and Proprietary Qualcomm Technologies, Inc.

// NO PUBLIC DISCLOSURE PERMITTED:  Please report postings of this software on public servers or websites
// to: DocCtrlAgent@qualcomm.com.

// RESTRICTED USE AND DISCLOSURE:
// This software contains confidential and proprietary information and is not to be used, copied, reproduced, modified
// or distributed, in whole or in part, nor its contents revealed in any manner, without the express written permission
// of Qualcomm Technologies, Inc.

// Qualcomm is a trademark of QUALCOMM Incorporated, registered in the United States and other countries. All
// QUALCOMM Incorporated trademarks are used with permission.

// This software may be subject to U.S. and international export, re-export, or transfer laws.  Diversion contrary to U.S.
// and international law is strictly prohibited.

// Qualcomm Technologies, Inc.
// 5775 Morehouse Drive
// San Diego, CA 92121 U.S.A.
// Copyright 2022-2023 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
