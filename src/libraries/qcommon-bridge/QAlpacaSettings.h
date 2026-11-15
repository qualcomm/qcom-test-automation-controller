#ifndef QALPACASETTINGS_H
#define QALPACASETTINGS_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonBridgeGlobal.h"
#include "QCommonTypes.h"
#include "AlpacaSettings.h"

// Qt
#include <QPoint>
#include <QSize>
#include <QString>
#include <cstdio>

class QCOMMONBRIDGE_EXPORT QAlpacaSettings : public AlpacaSettings
{
public:
	class QCOMMONBRIDGE_EXPORT QValue
	{
	public:
		QValue() = default;
		QValue(const INISettings::Value& v) : _val(v) {}
		QValue(const QString& s) : _val(toStdString(s)) {}
		QValue(const char* s) : _val(s ? std::string(s) : std::string()) {}
		QValue(int v) : _val(v) {}
		QValue(bool v) : _val(v) {}
		QValue(double v) : _val(v) {}

		QString toString() const { return toQString(_val.toString()); }
		int toInt(int defaultVal = 0) const { return _val.toInt(defaultVal); }
		bool toBool(bool defaultVal = false) const { return _val.toBool(defaultVal); }
		double toDouble(double defaultVal = 0.0) const { return _val.toDouble(defaultVal); }

		QSize toSize(const QSize& defaultVal = QSize()) const
		{
			std::string str = _val.toString();
			if (str.empty())
				return defaultVal;
			int w = 0, h = 0;
			if (sscanf(str.c_str(), "%dx%d", &w, &h) == 2 || sscanf(str.c_str(), "@Size(%d %d)", &w, &h) == 2)
				return QSize(w, h);
			return defaultVal;
		}

		QPoint toPoint(const QPoint& defaultVal = QPoint()) const
		{
			std::string str = _val.toString();
			if (str.empty())
				return defaultVal;
			int x = 0, y = 0;
			if (sscanf(str.c_str(), "%d,%d", &x, &y) == 2 || sscanf(str.c_str(), "@Point(%d %d)", &x, &y) == 2)
				return QPoint(x, y);
			return defaultVal;
		}

	private:
		INISettings::Value _val;
	};

	QAlpacaSettings() : AlpacaSettings() {}
	explicit QAlpacaSettings(const QString& appName) : AlpacaSettings(toStdString(appName)) {}
	QAlpacaSettings(const QString& org, const QString& appName) : AlpacaSettings(toStdString(org), toStdString(appName)) {}

	// setValue overloads
	void setValue(const QString& key, const QString& value)
	{
		INISettings::setValue(toStdString(key), toStdString(value));
	}
	void setValue(const QString& key, const char* value)
	{
		INISettings::setValue(toStdString(key), std::string(value));
	}
	void setValue(const QString& key, int value)
	{
		INISettings::setValue(toStdString(key), value);
	}
	void setValue(const QString& key, bool value)
	{
		INISettings::setValue(toStdString(key), value);
	}
	void setValue(const QString& key, double value)
	{
		INISettings::setValue(toStdString(key), value);
	}

	void setValue(const char* key, const QString& value)
	{
		INISettings::setValue(std::string(key), toStdString(value));
	}
	void setValue(const char* key, const char* value)
	{
		INISettings::setValue(std::string(key), std::string(value));
	}
	void setValue(const char* key, int value)
	{
		INISettings::setValue(std::string(key), value);
	}
	void setValue(const char* key, bool value)
	{
		INISettings::setValue(std::string(key), value);
	}
	void setValue(const char* key, double value)
	{
		INISettings::setValue(std::string(key), value);
	}

	// QSize support
	void setValue(const char* key, const QSize& size)
	{
		INISettings::setValue(std::string(key) + "/width", size.width());
		INISettings::setValue(std::string(key) + "/height", size.height());
		INISettings::setValue(std::string(key), std::to_string(size.width()) + "x" + std::to_string(size.height()));
	}
	void setValue(const QString& key, const QSize& size)
	{
		setValue(key.toUtf8().constData(), size);
	}

	// QPoint support
	void setValue(const char* key, const QPoint& point)
	{
		INISettings::setValue(std::string(key) + "/x", point.x());
		INISettings::setValue(std::string(key) + "/y", point.y());
		INISettings::setValue(std::string(key), std::to_string(point.x()) + "," + std::to_string(point.y()));
	}
	void setValue(const QString& key, const QPoint& point)
	{
		setValue(key.toUtf8().constData(), point);
	}

	// value overloads
	QValue value(const QString& key, const QValue& defaultValue = QValue()) const
	{
		return QValue(INISettings::value(toStdString(key), INISettings::Value()));
	}
	QValue value(const QString& key, const QString& defaultValue) const
	{
		return QValue(INISettings::value(toStdString(key), INISettings::Value(toStdString(defaultValue))));
	}
	QValue value(const QString& key, const char* defaultValue) const
	{
		return QValue(INISettings::value(toStdString(key), INISettings::Value(std::string(defaultValue))));
	}
	QValue value(const QString& key, int defaultValue) const
	{
		return QValue(INISettings::value(toStdString(key), INISettings::Value(defaultValue)));
	}
	QValue value(const QString& key, bool defaultValue) const
	{
		return QValue(INISettings::value(toStdString(key), INISettings::Value(defaultValue)));
	}

	QValue value(const char* key, const QValue& defaultValue = QValue()) const
	{
		return QValue(INISettings::value(std::string(key), INISettings::Value()));
	}
	QValue value(const char* key, const QString& defaultValue) const
	{
		return QValue(INISettings::value(std::string(key), INISettings::Value(toStdString(defaultValue))));
	}
	QValue value(const char* key, const char* defaultValue) const
	{
		return QValue(INISettings::value(std::string(key), INISettings::Value(std::string(defaultValue))));
	}
	QValue value(const char* key, int defaultValue) const
	{
		return QValue(INISettings::value(std::string(key), INISettings::Value(defaultValue)));
	}
	QValue value(const char* key, bool defaultValue) const
	{
		return QValue(INISettings::value(std::string(key), INISettings::Value(defaultValue)));
	}

	QValue value(const char* key, const QSize& defaultValue) const
	{
		std::string k(key);
		std::string str = INISettings::value(k).toString();
		if (!str.empty())
			return QValue(INISettings::Value(str));
		int w = INISettings::value(k + "/width", defaultValue.width()).toInt();
		int h = INISettings::value(k + "/height", defaultValue.height()).toInt();
		return QValue(INISettings::Value(std::to_string(w) + "x" + std::to_string(h)));
	}
	QValue value(const QString& key, const QSize& defaultValue) const
	{
		return value(key.toUtf8().constData(), defaultValue);
	}

	QValue value(const char* key, const QPoint& defaultValue) const
	{
		std::string k(key);
		std::string str = INISettings::value(k).toString();
		if (!str.empty())
			return QValue(INISettings::Value(str));
		int x = INISettings::value(k + "/x", defaultValue.x()).toInt();
		int y = INISettings::value(k + "/y", defaultValue.y()).toInt();
		return QValue(INISettings::Value(std::to_string(x) + "," + std::to_string(y)));
	}
	QValue value(const QString& key, const QPoint& defaultValue) const
	{
		return value(key.toUtf8().constData(), defaultValue);
	}

	// beginGroup
	void beginGroup(const QString& group)
	{
		INISettings::beginGroup(toStdString(group));
	}
	void beginGroup(const char* group)
	{
		INISettings::beginGroup(std::string(group));
	}
};

#endif // QALPACASETTINGS_H
