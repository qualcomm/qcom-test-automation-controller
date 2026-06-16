#ifndef UPDATEDEVICELIST_H
#define UPDATEDEVICELIST_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include <QString>

class UpdateDeviceList
{
public:
	UpdateDeviceList();
	void setDeviceListDir(const QString& fileDir);
	void setVerbosity(bool verbose);
    void write();

private:
	void save(const QString& filePath);

	QString                     _fileDir;
	bool                        _verbose{false};
};

#endif // UPDATEDEVICELIST_H
