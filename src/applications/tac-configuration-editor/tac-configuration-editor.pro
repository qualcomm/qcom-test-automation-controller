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

# Author: msimpson, biswroy

include(../../libraries/qcommon/Common.pri)

QT += core gui widgets serialport network xml

VERSION = $$TAC_CONFIG_VERSION
TARGET = TACConfigEditor
TEMPLATE = app
RC_ICONS = $$PWD/Resources/TACConfigEditor.ico

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
	ButtonEditor.cpp \
	CodeEditor.cpp \
	ConfigEditorApplication.cpp \
	ConfigWindow.cpp \
	CreateConfigurationDialog.cpp \
	EditorView.cpp \
	FTDIEditorView.cpp \
	ManageTabsDialog.cpp \
	PineCommandLine.cpp \
	TACPreviewWindow.cpp \
	TabEditorDialog.cpp \
	main.cpp

HEADERS += \
	ButtonEditor.h \
	CodeEditor.h \
	ConfigEditorApplication.h \
	ConfigWindow.h \
	CreateConfigurationDialog.h \
	EditorView.h \
	FTDIEditorView.h \
	ManageTabsDialog.h \
	PineCommandLine.h \
	TACPreviewWindow.h \
	TabEditorDialog.h

FORMS += \
	ButtonEditor.ui \
	CodeEditor.ui \
	ConfigWindow.ui \
	CreateConfigurationDialog.ui \
	FTDIEditorView.ui \
	ManageTabsDialog.ui \
	TACPreviewWindow.ui \
	TabEditorDialog.ui

CONFIG(debug, debug|release) {
	LIBS += -L$$STATIC_LIBPATH -lUId -lQCommond -lQCommon-consoled
} else {
	LIBS += -L$$STATIC_LIBPATH -lUI -lQCommon -lQCommon-console
}

LIBS += -L$$STATIC_LIBPATH -lftd2xx

unix {
	DISTFILES += \
		../LinuxDeploy/TACConfigEditor.desktop

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
	resources/TACConfigEditor.qrc
