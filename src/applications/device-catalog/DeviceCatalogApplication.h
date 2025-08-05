#ifndef DEVICECATALOGAPPLICATION_H
#define DEVICECATALOGAPPLICATION_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
    Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/


#include "AlpacaApplication.h"
#include "DeviceCatalog.h"
#include "version.h"

const QByteArray kAppName{QByteArrayLiteral("Device Catalog")};
const QByteArray kAppVersion{QByteArrayLiteral(DEVICE_CATALOG_VERSION)};


class DeviceCatalogApplication : public AlpacaApplication
{
    Q_OBJECT

public:
    DeviceCatalogApplication(int &argc, char **argv);
    ~DeviceCatalogApplication();

    static DeviceCatalog* createCatalogWindow();

    static DeviceCatalogApplication* appInstance();

    PreferencesBase* preferences()
    {
        return &_preferences;
    }

protected:
    void shutDown();

    PreferencesBase				_preferences;
};

#endif // DEVICECATALOGAPPLICATION_H
