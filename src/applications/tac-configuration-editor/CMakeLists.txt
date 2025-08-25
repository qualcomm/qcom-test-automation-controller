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

# Author: Biswajit Roy <biswroy@qti.qualcomm.com>

qtac_add_application(
    TARGET TACConfigEditor
    OUTPUT_NAME TACConfigEditor
    WIN32_EXECUTABLE
    SOURCES
        main.cpp
        ButtonEditor.cpp
        CodeEditor.cpp
        ConfigEditorApplication.cpp
        ConfigWindow.cpp
        CreateConfigurationDialog.cpp
        EditorView.cpp
        FTDIEditorView.cpp
        ManageTabsDialog.cpp
        TabEditorDialog.cpp
        TACPreviewWindow.cpp
        tac-configuration-editor.rc
    HEADERS
        ButtonEditor.h
        CodeEditor.h
        ConfigEditorApplication.h
        ConfigWindow.h
        CreateConfigurationDialog.h
        EditorView.h
        FTDIEditorView.h
        ManageTabsDialog.h
        TabEditorDialog.h
        TACPreviewWindow.h
    UI_FILES
        ButtonEditor.ui
        CodeEditor.ui
        ConfigWindow.ui
        CreateConfigurationDialog.ui
        FTDIEditorView.ui
        ManageTabsDialog.ui
        TabEditorDialog.ui
        TACPreviewWindow.ui
    QT_COMPONENTS
        Core Gui Widgets
    LIBRARIES
        UI QCommon QCommon-console
    SYSTEM_LIBRARIES
        Advapi32 Shell32 Wtsapi32
)

# Add resources if they exist
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/resources")
    file(GLOB_RECURSE RESOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/resources/*")
    if(RESOURCE_FILES)
        qt6_add_resources(tac-configuration-editor "tac-config-editor-resources"
            PREFIX "/"
            BASE "${CMAKE_CURRENT_SOURCE_DIR}"
            FILES ${RESOURCE_FILES}
        )
    endif()
endif()
