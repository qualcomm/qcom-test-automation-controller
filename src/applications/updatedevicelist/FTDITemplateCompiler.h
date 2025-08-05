#ifndef FTDITEMPLATECOMPILER_H
#define FTDITEMPLATECOMPILER_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "FTDIPinSet.h"

// Qt
#include <QString>

class FTDITemplateCompiler
{
public:
	FTDITemplateCompiler();
	void setDeviceListDir(const QString& fileDir);
	void setVerbosity(bool verbose);
	void write();

	// Read configuration's USB descriptor string. Read VCP and D2XX bus lines
	// Update template and save a copy
	// Allow APIs to extend programming debug boards using Device Catalog. Maybe in a FTDIProgrammer class.
private:
	bool load();
	void save(const QString& xmlDocumentName, const QString &usbDescriptorString, FTDIPinSets *pinSets);

	QString						_fileDir;
	bool						_verbose{false};
	QString						_template;
	QString						_lastError;
};

#endif // FTDITEMPLATECOMPILER_H
