// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "ScriptVariable.h"

#include <cctype>

static bool equalsCaseInsensitive(const std::string& left, const std::string& right)
{
	if (left.size() != right.size())
		return false;

	for (size_t index = 0; index < left.size(); index++)
	{
		if (std::tolower(static_cast<unsigned char>(left[index])) != std::tolower(static_cast<unsigned char>(right[index])))
			return false;
	}

	return true;
}

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
