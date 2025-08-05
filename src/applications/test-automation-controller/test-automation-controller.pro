# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries. 
#  
# Redistribution and use in source and binary forms, with or without
# modification, are permitted (subject to the limitations in the
# disclaimer below) provided that the following conditions are met:
#  
# 	* Redistributions of source code must retain the above copyright
# 	  notice, this list of conditions and the following disclaimer.
#  
# 	* Redistributions in binary form must reproduce the above
# 	  copyright notice, this list of conditions and the following
# 	  disclaimer in the documentation and/or other materials provided
# 	  with the distribution.
#  
# 	* Neither the name of Qualcomm Technologies, Inc. nor the names of its
# 	  contributors may be used to endorse or promote products derived
# 	  from this software without specific prior written permission.
#  
# NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
# GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
# HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
# GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
# IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
# IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# Author: msimpson

include(../../libraries/qcommon/Common.pri)

QT += core gui widgets serialport network

VERSION = $$TAC_VERSION

TARGET = TAC
TEMPLATE = app
RC_ICONS = $$PWD/resources/TAC.ico

DESTDIR = $$BINPATH

INCLUDEPATH += $$QCOMMON
INCLUDEPATH += $$QCOMMONCONSOLE
INCLUDEPATH += $$LIBUI

DEPENDPATH += $$QCOMMON
DEPENDPATH += $$QCOMMONCONSOLE
DEPENDPATH += $$LIBUI

win32 {
	CONFIG(debug, debug|release) {
	PRE_TARGETDEPS += $$STATIC_LIBPATH/QCommond.lib
	PRE_TARGETDEPS += $$STATIC_LIBPATH/qcommon-consoled.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/UId.lib
	} else {
	PRE_TARGETDEPS += $$STATIC_LIBPATH/QCommon.lib
	PRE_TARGETDEPS += $$STATIC_LIBPATH/qcommon-console.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/UI.lib
	}
}

linux {
	CONFIG(debug, debug|release) {
	PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommond.a
	PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommon-consoled.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libUId.a
	} else {
	PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommon.a
	PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommon-console.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libUI.a
	}
}

SOURCES += \
	main.cpp \
	PreferencesDialog.cpp \
	TACApplication.cpp \
	TACDeviceSelection.cpp \
	TACWindow.cpp

HEADERS += \
	PreferencesDialog.h \
	TACApplication.h \
	TACDeviceSelection.h \
	TACWindow.h

FORMS += \
	PreferencesDialog.ui \
	TACDeviceSelection.ui \
	TACWindow.ui


CONFIG(debug, debug|release) {
        LIBS += -L$$STATIC_LIBPATH -lUId -lQCommond -lQCommon-consoled
} else {
        LIBS += -L$$STATIC_LIBPATH -lUI -lQCommon -lQCommon-console
}

LIBS += -L$$STATIC_LIBPATH -lftd2xx

win32 {
	LIBS += -lSetupAPI -lWinusb -lOle32 -lAdvapi32 -lShell32 -lWtsapi32
}

unix {
	DISTFILES += \
		../LinuxDeploy/TAC.desktop

	LIBS += -rdynamic
	linux {
		LIBS += -L/usr/local/lib

		LIBS += -lrt
		LIBS += -ldl
		LIBS += -luuid
	}
	mac {
		QMAKE_LFLAGS += -F/System/Library/Frameworks/CoreFoundation.framework
		QMAKE_LFLAGS += -F/System/Library/Frameworks/IOKit.framework
		QMAKE_LFLAGS += -framework Carbon
		ICON = $$PWD/resources/qxdm.icns
		LIBS += -framework CoreFoundation
		LIBS += -framework IOKit
		LIBS += -lobjc

		QMAKE_POST_LINK = $$PWD/debug_plugin.sh $$TARGET $$BINPATH
	}
}

RESOURCES += \
	resources/TAC.qrc
