#ifndef SCRIPTVARIABLE_H
#define SCRIPTVARIABLE_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"
#include "StringUtilities.h"

#include <map>
#include <string>
#include <utility>
#include <variant>

const std::string kDefaultVariableName{"<variable name>"};
const std::string kDefaultVariableLabel{"<label for the variable>"};
const std::string kDefaultVariableTooltip{"<tooltip for the variable>"};

enum VariableType
{
	eUnknownVariableType = 0,
	eIntegerType,
    eBooleanType,
	eFloatType
};

typedef std::variant<std::monostate, bool, uint32_t, std::string> ScriptVariableValue;

struct QCOMMONCONSOLE_EXPORT ScriptVariable
{
public:
	ScriptVariable() = default;
	~ScriptVariable() = default;
	ScriptVariable(const ScriptVariable& copyMe) = default;

	std::string					_name{kDefaultVariableName};
	std::string					_label{kDefaultVariableLabel};
	std::string					_tooltip{kDefaultVariableTooltip};
	VariableType				_type{eUnknownVariableType};
	ScriptVariableValue			_defaultValue{static_cast<uint32_t>(0)};
	std::pair<int, int>			_cellLocation{-1,-1};
};

typedef std::map<std::string, ScriptVariable> ScriptVariables;

std::string QCOMMONCONSOLE_EXPORT variableTypeToString(VariableType variableType);
VariableType QCOMMONCONSOLE_EXPORT variableTypeFromString(const std::string& typeString);

#endif // SCRIPTVARIABLE_H
