// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

// Author: Biswajit Roy (biswroy@qti.qualcomm.com)

#include "ScriptVariable.h"

QString variableTypeToString(VariableType variableType)
{
	QString result("Unknown");

	switch (variableType)
	{
	case eIntegerType: result = "Integer"; break;
    case eBooleanType: result = "Boolean"; break;
	case eFloatType: result = "Float"; break;
	default:
		break;
	}

	return result;

}

VariableType variableTypeFromString(const QString& boardString)
{
	VariableType result{eUnknownVariableType};

	if (boardString.compare("Integer", Qt::CaseInsensitive) == 0)
        result = eIntegerType;
    else if (boardString.compare("Boolean", Qt::CaseInsensitive) == 0)
        result = eBooleanType;
	else if (boardString.compare("Float", Qt::CaseInsensitive) == 0)
		result = eFloatType;
	return result;
}
