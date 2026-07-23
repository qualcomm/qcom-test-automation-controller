// Copyright (c) 2018-2023 Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
    Author: Michael Simpson (msimpson@qti.qualcomm.com)
            Biswajit Roy (biswroy@qti.qualcomm.com)
*/

// Qt
#include <QCoreApplication>
#include <QSettings>

// Windows
#ifdef Q_OS_WINDOWS
    #include <Windows.h>
#endif
// C++
#include <iostream>

using namespace std;

static bool runningAsAdmin()
{
    bool isAdmin = false;
#ifdef Q_OS_WINDOWS
    HANDLE hProcessToken = NULL;
    HANDLE hProcess = GetCurrentProcess();

    if (OpenProcessToken(hProcess, TOKEN_QUERY, &hProcessToken))
    {
        char AdminSID[SECURITY_MAX_SID_SIZE];
        DWORD dwLength = sizeof(AdminSID);

        if (CreateWellKnownSid(WinBuiltinAdministratorsSid, NULL, &AdminSID, &dwLength))
        {
            BOOL admin;
            CheckTokenMembership(NULL, &AdminSID, &admin);

            isAdmin = (admin == TRUE);
        }

        CloseHandle(hProcessToken);
    }

    CloseHandle(hProcess);
#endif
    return isAdmin == true;
}

static void cleanEntries(QSettings& settings)
{
    cout << std::endl << std::endl << "Cleaning " << (settings.fileName() + "\\" + settings.group()).toLatin1().data() << std::endl;

    auto childGroups = settings.childGroups();
    for (const auto& childGroup: childGroups)
    {
        if (childGroup.contains("-1962-") == true || childGroup.contains("-1998-") == true)
        {
            settings.remove(childGroup);
            cout << "Entry:" << childGroup.toLatin1().data() << " removed." << std::endl;
        }
        else if (childGroup.startsWith("EPM.", Qt::CaseInsensitive) == true)
        {
            settings.remove(childGroup);
            cout << "Entry:" << childGroup.toLatin1().data() << " removed." << std::endl;
        }
        else if (childGroup.startsWith("TAC.", Qt::CaseInsensitive) == true)
        {
            settings.remove(childGroup);
            cout << "Entry:" << childGroup.toLatin1().data() << " removed." << std::endl;
        }
        else if (childGroup.startsWith("TACCOM.", Qt::CaseInsensitive) == true)
        {
            settings.remove(childGroup);
            cout << "Entry:" << childGroup.toLatin1().data() << " removed." << std::endl;
        }
    }
}

int main(int argc, char *argv[])
{
    if (!runningAsAdmin())
    {
        cout << "Registry Cleaner must be run as Administrator" << std::endl;
        return 0;
    }

    QCoreApplication a(argc, argv);

    QSettings userAppIDSettings("HKEY_CURRENT_USER\\Software\\Classes\\AppID", QSettings::NativeFormat);
    cleanEntries(userAppIDSettings);

    QSettings hkcrInterface("HKEY_CLASSES_ROOT\\Interface", QSettings::NativeFormat);
    cleanEntries(hkcrInterface);

    QSettings hkcrWOWInterface("HKEY_CLASSES_ROOT\\WOW6432Node\\Interface", QSettings::NativeFormat);
    cleanEntries(hkcrWOWInterface);

    QSettings machineClassSettings("HKEY_LOCAL_MACHINE\\SOFTWARE\\Classes", QSettings::NativeFormat);
    cleanEntries(machineClassSettings);

    machineClassSettings.beginGroup("AppID");
    cleanEntries(machineClassSettings);
    machineClassSettings.endGroup();

    machineClassSettings.beginGroup("CLSID");
    cleanEntries(machineClassSettings);
    machineClassSettings.endGroup();

    machineClassSettings.beginGroup("Interface");
    cleanEntries(machineClassSettings);
    machineClassSettings.endGroup();

    machineClassSettings.beginGroup("TypeLib");
    cleanEntries(machineClassSettings);
    machineClassSettings.endGroup();

    machineClassSettings.beginGroup("WOW6432Node\\Interface");
    cleanEntries(machineClassSettings);
    machineClassSettings.endGroup();

    cout << "Finished!" << std::endl;
}