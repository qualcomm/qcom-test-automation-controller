# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause-Clear
# Maintainer: Biswajit Roy (biswroy@qti.qualcomm.com)
# Original author: Michael Simpson (msimpson@qti.qualcomm.com)

include(version.pri)

QT -= gui

DEFINES += QT_DISABLE_DEPRECATED_UP_TO=0x060600    # disables all the APIs deprecated before Qt 6.6.0

QCOMMONCONSOLE = "$$PWD"
TACDEV = "$$PWD/../TACDev"

CONFIG += skip_target_version_ext

DEFINES += QTAC_STATIC

win32 {
	CONFIG += c++20
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
	win32 {
		QMAKE_CXXFLAGS += /Zi
		CONFIGURATION = "$$WINTARGET/Debug"
	}
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

unix:!mac {
#	QT_STATICLIBS = "/local/mnt/workspace/static/lib"
#	LIBS = -L$$QT_STATICLIBS -lQt5Core -lqtpcre2
	QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN
	QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN/lib
	QMAKE_LFLAGS += -fPIC
	QMAKE_RPATH=
	QMAKE_CXXFLAGS_WARN_ON += -Wno-pragmas
}

unix: {
	QMAKE_LFLAGS_RPATH=
	QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN
	QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN/../lib
	QMAKE_LFLAGS += -fPIC
	QMAKE_CXXFLAGS_WARN_ON += -Wno-pragmas
	QMAKE_CXXFLAGS += -Werror -Wno-unused-result -Wno-write-strings -Wno-comment -Wno-unused-function -Wno-reorder -Wno-unknown-pragmas -Wno-conversion-null -std=c++0x
	QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder -Wno-unused-parameter -Wno-overloaded-virtual -Wno-unused-function -Wno-unused-variable -Wno-date-time
}

unix:mac {
#for xcode 7, use this one.
   #QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-private-field -Wno-inconsistent-missing-override
QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-private-field
}

QMAKE_TARGET_COMPANY="Qualcomm, Inc."
QMAKE_TARGET_COPYRIGHT="2025 Qualcomm, Inc."
QMAKE_TARGET_DESCRIPTION="Qualcomm Test Automation Controller Application Suite"
