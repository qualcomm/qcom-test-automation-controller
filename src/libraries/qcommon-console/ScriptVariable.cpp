// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "ScriptVariable.h"
#include "StringUtilities.h"

#include <cctype>

std::string variableTypeToString(VariableType variableType)
{
	std::string result("Unknown");

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

VariableType variableTypeFromString(const std::string& boardString)
{
	VariableType result{eUnknownVariableType};

	if (equalsCaseInsensitive(boardString, "Integer"))
        result = eIntegerType;
    else if (equalsCaseInsensitive(boardString, "Boolean"))
        result = eBooleanType;
	else if (equalsCaseInsensitive(boardString, "Float"))
		result = eFloatType;
	return result;
}
