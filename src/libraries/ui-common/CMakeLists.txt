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

# Find Qt components
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets SerialPort)

# Define the library target
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(TARGET_NAME UId)
else()
    set(TARGET_NAME UI)
endif()

add_library(${TARGET_NAME} STATIC
    # Source files
    ColorConversion.cpp
    CommandButton.cpp
    CustomValidator.cpp
    HoverAwareQWindow.cpp
    I2CWidget.cpp
    NotificationWidget.cpp
    TACFrame.cpp
    TerminalWidget.cpp
    UserWidget.cpp
    UILib.cpp
    VariableInput.cpp
)

# Add headers
target_sources(${TARGET_NAME} PRIVATE
    ColorConversion.h
    CommandButton.h
    CustomValidator.h
    HoverAwareQWindow.h
    I2CWidget.h
    NotificationWidget.h
    TACFrame.h
    TerminalWidget.h
    UserWidget.h
    UILib.h
    UIGlobalLib.h
    VariableInput.h
)

# Process UI files if they exist
set(UI_FILES
    HoverAwareQWindow.ui
    I2CWidget.ui
    NotificationWidget.ui
    TACFrame.ui
    TerminalWidget.ui
    UserWidget.ui
    VariableInput.ui
)

# Filter UI files to only include existing ones
set(EXISTING_UI_FILES)
foreach(ui_file ${UI_FILES})
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${ui_file}")
        list(APPEND EXISTING_UI_FILES ${ui_file})
    endif()
endforeach()

# Add UI resources if any exist
if(EXISTING_UI_FILES)
    qt6_add_resources(${TARGET_NAME} "ui-common-ui"
        PREFIX "/ui"
        FILES ${EXISTING_UI_FILES}
    )
endif()

# Add resources if they exist
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/resources/ui-common.qrc")
    qt6_add_resources(${TARGET_NAME} "ui-common-resources"
        PREFIX "/"
        FILES
            resources/ui-common.qrc
    )
endif()

# Set target properties
set_target_properties(${TARGET_NAME} PROPERTIES
    OUTPUT_NAME ${TARGET_NAME}
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
)

# Link Qt libraries
target_link_libraries(${TARGET_NAME} PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::SerialPort
)

# Link to other libraries
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(${TARGET_NAME} PUBLIC 
        QCommond
        QCommon-consoled
    )
else()
    target_link_libraries(${TARGET_NAME} PUBLIC 
        QCommon
        QCommon-console
    )
endif()

# Include directories - PUBLIC so dependent targets inherit them
target_include_directories(${TARGET_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/ui-common>
)

# Export target for use by other parts of the project
set_target_properties(${TARGET_NAME} PROPERTIES EXPORT_NAME UICommon)
