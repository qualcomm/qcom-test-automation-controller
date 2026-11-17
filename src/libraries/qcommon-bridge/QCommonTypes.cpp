// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonTypes.h"
#include <type_traits>

namespace QTAC
{
namespace Types
{
	QStringList toQStringList(const std::vector<std::string>& vec)
	{
		QStringList list;
		list.reserve(static_cast<qsizetype>(vec.size()));
		for (const auto& str : vec)
		{
			list.append(toQString(str));
		}
		return list;
	}

	std::vector<std::string> toStdVector(const QStringList& list)
	{
		std::vector<std::string> vec;
		vec.reserve(static_cast<size_t>(list.size()));
		for (const auto& qstr : list)
		{
			vec.push_back(toStdString(qstr));
		}
		return vec;
	}

	QVariant toQVariant(const ScriptVariableValue& val)
	{
		return std::visit([](auto&& arg) -> QVariant {
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, std::monostate>)
			{
				return QVariant();
			}
			else if constexpr (std::is_same_v<T, bool>)
			{
				return QVariant(arg);
			}
			else if constexpr (std::is_same_v<T, uint32_t>)
			{
				return QVariant(arg);
			}
			else if constexpr (std::is_same_v<T, std::string>)
			{
				return QVariant(toQString(arg));
			}
			else
			{
				return QVariant();
			}
		}, val);
	}

	ScriptVariableValue toScriptVariableValue(const QVariant& qval, VariableType type)
	{
		switch (type)
		{
		case eBooleanType:
			return qval.toBool();
		case eIntegerType:
			return static_cast<uint32_t>(qval.toUInt());
		case eFloatType:
			return toStdString(qval.toString());
		default:
			return toStdString(qval.toString());
		}
	}
}
}
