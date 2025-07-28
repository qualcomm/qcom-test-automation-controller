# Qualcomm is a trademark of QUALCOMM Incorporated, registered in the United States and other countries. All
# QUALCOMM Incorporated trademarks are used with permission.

# This software may be subject to U.S. and international export, re-export, or transfer laws.  Diversion contrary to U.S.
# and international law is strictly prohibited.

# Qualcomm Technologies, Inc.
# 5775 Morehouse Drive
# San Diego, CA 92121 U.S.A.
# Copyright © 2013-2024 Qualcomm Technologies, Inc.
# All rights reserved.
# Qualcomm Technologies Confidential and Proprietary
#
include($$PWD/../qcommon-console/version.pri)

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

QCOMMON = "$$PWD"

QCOMMONCONSOLE = "$$PWD/../qcommon-console"
LIBUI = "$$PWD/../ui-common"

TACDEV = "$$PWD/../../../interfaces/C++/TACDev"

TACLOC = "$$PWD/../TAC"

CONFIG += skip_target_version_ext

DEFINES += QTAC_STATIC

message($$QCOMMONCONSOLE)

win32 {
	CONFIG += c++20
	QMAKE_CXXFLAGS += /Zi
    QMAKE_LFLAGS += /INCREMENTAL:NO
}

linux {
    CONFIG += c++2a c++1z c++14
}

CONFIG(x32) {
    message(Building 32 bit)
	win32: DEFINES += _WIN32
    linux: DEFINES += __i386__
	WINTARGET = "Win32"
} else {
    message(Building 64 bit)
    linux: DEFINES += __X86_64__
	WINTARGET = "x64"
}

CONFIG(debug, debug|release) {
	DEFINES += _DEBUG DEBUG
	win32: CONFIGURATION = "$$WINTARGET/Debug"
	linux: CONFIGURATION = "Linux/Debug"
}
else {
	DEFINES += _NDEBUG NDEBUG
	win32: CONFIGURATION = "$$WINTARGET/Release"
	linux: CONFIGURATION = "Linux/Release"
}

BUILDROOT = $$PWD/../../../__Builds/$$CONFIGURATION
BINPATH = "$$BUILDROOT/bin"
DESTDIR = $$BINPATH
OBJECTS_DIR = "$$BUILDROOT/obj/$$TARGET"
STATIC_LIBPATH = "$$BUILDROOT/lib"
MOC_DIR = "$$BUILDROOT/moc/$$TARGET"
UI_DIR = "$$BUILDROOT/ui/$$TARGET"

INCLUDEPATH += "$$BUILDROOT/ui/QCommon"


unix: {
	QMAKE_CXXFLAGS += -Werror -Wno-unused-result -Wno-write-strings -Wno-comment -Wno-unused-function -Wno-reorder -Wno-unknown-pragmas -Wno-conversion-null -std=c++0x
	QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder -Wno-unused-parameter -Wno-overloaded-virtual -Wno-unused-function -Wno-unused-variable -Wno-date-time
}

QMAKE_TARGET_COMPANY="Qualcomm, Inc."
QMAKE_TARGET_COPYRIGHT="2025 Qualcomm, Inc."
QMAKE_TARGET_DESCRIPTION="Qualcomm Test Automation Controller Application Suite"
