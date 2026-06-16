# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

include(../QCommonConsole/Common.pri)

QT += core

TARGET = PSOCProgrammer
TEMPLATE = app

CONFIG += console
CONFIG -= app_bundle

DESTDIR = $$BINPATH

INCLUDEPATH += $$QCOMMONCONSOLE
INCLUDEPATH += $$LIBEPM

DEPENDPATH += $$QCOMMONCONSOLE
DEPENDPATH += $$LIBEPM

win32 {

	QT += axcontainer # for Excel operations

	CONFIG(debug, debug|release) {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/QCommonConsoled.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/EPMLibd.lib
	} else {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/QCommonConsole.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/EPMLib.lib
	}
}

linux {
	CONFIG(debug, debug|release) {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommonConsoled.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libEPMLibd.a
	} else {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommonConsole.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libEPMLib.a
	}
}

HEADERS += \
	PSOCProgrammerCmdLine.h

SOURCES += \
	PSOCProgrammerCmdLine.cpp \
	main.cpp

CONFIG(debug, debug|release) {
	LIBS += -L$$STATIC_LIBPATH -lEPMLibd -lQCommonConsoled
} else {
	LIBS += -L$$STATIC_LIBPATH -lEPMLib -lQCommonConsole
}

win32 {
	LIBS += -lSetupAPI -lWinusb
}

unix {
	LIBS += -lusb
}
