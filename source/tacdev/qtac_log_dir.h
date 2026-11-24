// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause
//
// Pre-Qt helper: resolve the QTAC log directory for a given sub-component
// without depending on Qt (safe to call before QCoreApplication exists).
//
// Mirrors the path logic in the legacy AppCore / ConsoleApplicationEnhancements:
//   Windows: %USERPROFILE%\Documents\QTAC\<component>\logs
//   Linux:   $XDG_DOCUMENTS_DIR/QTAC/<component>/logs
//            (falls back to $HOME/Documents, then $HOME if XDG not set)
//
// Returns an empty string when no writable location can be determined.
// The caller should fall back to CWD in that case.

#pragma once

#include <string>

#ifdef _WIN32
#  include <windows.h>
#else
#  include <cstdlib>
#  include <sys/stat.h>
#  include <unistd.h>
#  include <pwd.h>
#endif

inline std::string qtacLogDir(const char* component)
{
#ifdef _WIN32
    char buf[MAX_PATH] = {};
    if (GetEnvironmentVariableA("USERPROFILE", buf, MAX_PATH) == 0)
        return {};
    std::string base(buf);
    std::string dir = base + "\\Documents\\QTAC\\" + component + "\\logs";
    // Create each component of the path (CreateDirectory is a no-op if it exists).
    for (size_t pos = base.size() + 1; ; )
    {
        pos = dir.find('\\', pos);
        std::string sub = (pos == std::string::npos) ? dir : dir.substr(0, pos);
        CreateDirectoryA(sub.c_str(), nullptr);
        if (pos == std::string::npos) break;
        ++pos;
    }
    return dir;
#else
    // Match Qt's QStandardPaths::DocumentsLocation resolution order:
    //   1. $XDG_DOCUMENTS_DIR (set by xdg-user-dirs, present on most desktops)
    //   2. $HOME/Documents    (common convention even without XDG)
    //   3. $HOME              (last resort — same as Qt's fallback)
    std::string docs;
    const char* xdg = getenv("XDG_DOCUMENTS_DIR");
    if (xdg && xdg[0] != '\0')
    {
        docs = xdg;
    }
    else
    {
        const char* home = getenv("HOME");
        if (!home)
        {
            struct passwd* pw = getpwuid(getuid());
            if (pw) home = pw->pw_dir;
        }
        if (!home) return {};

        // Prefer ~/Documents if it already exists; fall back to $HOME.
        std::string homeDocuments = std::string(home) + "/Documents";
        struct stat st{};
        docs = (stat(homeDocuments.c_str(), &st) == 0 && S_ISDIR(st.st_mode))
               ? homeDocuments
               : std::string(home);
    }

    std::string dir = docs + "/QTAC/" + component + "/logs";
    // Create each component of the path.
    std::string cur;
    for (char c : dir + "/")
    {
        cur += c;
        if (c == '/')
            mkdir(cur.c_str(), 0755);
    }
    return dir;
#endif
}
