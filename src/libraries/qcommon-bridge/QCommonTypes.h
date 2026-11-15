#ifndef QCOMMONTYPES_H
#define QCOMMONTYPES_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonBridgeGlobal.h"

// Qt
#include <QString>
#include <QStringList>
#include <QPoint>
#include <QSize>
#include <QVariant>
#include <QByteArray>

// Standard C++
#include <string>
#include <string_view>
#include <vector>
#include <utility>

// QCommonConsole
#include "ScriptVariable.h"
#include "DebugBoardType.h"

namespace QTAC
{
namespace Types
{
	// String conversions
	inline QString toQString(const std::string& str)
	{
		return QString::fromUtf8(str.data(), static_cast<qsizetype>(str.size()));
	}

	inline QString toQString(std::string_view sv)
	{
		return QString::fromUtf8(sv.data(), static_cast<qsizetype>(sv.size()));
	}

	inline QString toQString(const char* cstr)
	{
		return cstr ? QString::fromUtf8(cstr) : QString();
	}

	inline const QString& toQString(const QString& str)
	{
		return str;
	}

	inline QString toQString(const QByteArray& qba)
	{
		return QString::fromUtf8(qba);
	}

	inline std::string toStdString(const QString& qstr)
	{
		const QByteArray utf8 = qstr.toUtf8();
		return std::string(utf8.constData(), static_cast<size_t>(utf8.size()));
	}

	inline std::string toStdString(const QByteArray& qba)
	{
		return std::string(qba.constData(), static_cast<size_t>(qba.size()));
	}

	inline const std::string& toStdString(const std::string& str)
	{
		return str;
	}

	// List conversions
	QCOMMONBRIDGE_EXPORT QStringList toQStringList(const std::vector<std::string>& vec);
	QCOMMONBRIDGE_EXPORT std::vector<std::string> toStdVector(const QStringList& list);

	// Geometry conversions
	inline QPoint toQPoint(const std::pair<int, int>& pt)
	{
		return QPoint(pt.first, pt.second);
	}

	inline std::pair<int, int> toStdPair(const QPoint& pt)
	{
		return {pt.x(), pt.y()};
	}

	inline std::pair<int, int> toStdPair(const QSize& sz)
	{
		return {sz.width(), sz.height()};
	}

	inline QSize toQSize(const std::pair<int, int>& pt)
	{
		return QSize(pt.first, pt.second);
	}

	// ScriptVariable conversions
	QCOMMONBRIDGE_EXPORT QVariant toQVariant(const ScriptVariableValue& val);
	QCOMMONBRIDGE_EXPORT ScriptVariableValue toScriptVariableValue(const QVariant& qval, VariableType type);

	inline PSOCVariant psocVariantFromString(const QString& str)
	{
		return ::psocVariantFromString(toStdString(str));
	}

	inline QString toQString(DebugBoardType type)
	{
		return toQString(::debugBoardTypeToString(type));
	}
}
}

// Transparent string & stream operators for std::string and Qt
inline QString operator+(const QString& qstr, const std::string& sstr)
{
	return qstr + QTAC::Types::toQString(sstr);
}

inline QString operator+(const std::string& sstr, const QString& qstr)
{
	return QTAC::Types::toQString(sstr) + qstr;
}

inline QStringList& operator<<(QStringList& list, const char* cstr)
{
	list.append(QString::fromUtf8(cstr));
	return list;
}

inline QStringList& operator<<(QStringList& list, const QByteArray& qba)
{
	list.append(QString::fromUtf8(qba));
	return list;
}

inline QStringList& operator<<(QStringList& list, const std::string& sstr)
{
	list.append(QTAC::Types::toQString(sstr));
	return list;
}

// Global inline aliases for convenience
using QTAC::Types::toQString;
using QTAC::Types::toStdString;
using QTAC::Types::toQStringList;
using QTAC::Types::toStdVector;
using QTAC::Types::toQPoint;
using QTAC::Types::toStdPair;
using QTAC::Types::toQSize;
using QTAC::Types::toQVariant;
using QTAC::Types::toScriptVariableValue;
using QTAC::Types::psocVariantFromString;

#endif // QCOMMONTYPES_H
