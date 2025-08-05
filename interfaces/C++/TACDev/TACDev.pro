# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause-Clear

# Author: msimpson

include(../../../src/libraries/qcommon-console/Common.pri)

QT += core serialport network

TEMPLATE = lib
CONFIG += dll
DESTDIR = $$STATIC_LIBPATH

DEFINES += TACDEV_LIBRARY

INCLUDEPATH += $$QCOMMONCONSOLE

DEPENDPATH += $$QCOMMONCONSOLE

CONFIG(debug, debug|release) {
	TARGET = TACDevd
} else {
	TARGET = TACDev
}

DLLDESTDIR = $$BINPATH

win32 {
	CONFIG(debug, debug|release) {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/qcommon-consoled.lib
	} else {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/qcommon-console.lib
	}
}

linux {
	CONFIG(debug, debug|release) {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommon-consoled.a
	} else {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommon-console.a
	}
}

SOURCES += \
	TACDevCore.cpp \
	TACDev.cpp

HEADERS += \
	TACDevCore.h \
	TACDev.h

CONFIG(debug, debug|release) {
	LIBS += -L$$STATIC_LIBPATH -lQCommon-consoled
} else {
	LIBS += -L$$STATIC_LIBPATH -lQCommon-console
}

LIBS += -L$$STATIC_LIBPATH -lftd2xx

win32 {
	LIBS += -lSetupAPI -lWinusb -lOle32 -lAdvapi32 -lShell32 -lWtsapi32
}
