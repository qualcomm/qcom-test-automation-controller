#ifndef TACPREFERENCES_H
#define TACPREFERENCES_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"

#include <cstdint>
#include <string>

// QCommonConsole
#include "PreferencesBase.h"

class QCOMMONCONSOLE_EXPORT TACPreferences :
	public PreferencesBase
{
public:
	TACPreferences() = default;

	virtual void setAppName(const std::string& appName, const std::string& appVersion);

	int defaultPowerDelay();
	int powerDelay();
	void setPowerDelay(int powerDelay);
	void savePowerDelay(int powerDelay);

	int defaultButtonAssertTime();
	int buttonAssertTime();
	void setButtonAssertTime(int buttonAssertTime);
	void saveButtonAssertTime(int buttonAssertTime);

	bool defaultAutoShutdown();
	bool autoShutdownActive();
	void setAutoShutdownActive(bool shutdownActive);
	void saveAutoShutdownActive(bool shutdownActive);

	double defaultAutoShutdownDelay();
	double autoShutdownDelay();
	void setAutoShutdownDelay(double delayInHours);
	void saveAutoShutdownDelay(double delayInHours);
	int64_t autoShutdownDelayInMSecs();

	bool defaultOpenLastStart();
	bool openLastStart();
	void setOpenLastStart(bool openLastOnStart);
	void saveOpenLastStart(bool openLastOnStart);

	std::string lastDevice();
	void saveLastDevice(const std::string& lastDevice);

	std::string defaultTACEditorConfigLocation();
	std::string tacEditorConfigLocation();
	void setTACEditorConfigLocation(const std::string& tacConfigLocation);
	void saveTACEditorConfigLocation(const std::string& saveLocation);

private:
	int							_powerDelay{0};
	int							_buttonAssertTime{0};
	bool						_autoShutdown{false};
	double						_autoShutdownDelay{0.};
	bool						_openLast{false};
	std::string                  _tacEditorConfigLocation;
};

#endif // TACPREFERENCES_H
