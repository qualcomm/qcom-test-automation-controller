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
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Xml SerialPort Concurrent Network)

# Define the library target
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(TARGET_NAME QCommond)
else()
    set(TARGET_NAME QCommon)
endif()

add_library(${TARGET_NAME} STATIC
    # Source files
    AboutDialog.cpp
    AlpacaApplication.cpp
    ApplicationEnhancements.cpp
    ColorSwatch.cpp
    HappinessDialog.cpp
    IntFilteredEditor.cpp
    LEDWidget.cpp
    PainterEffects.cpp
    PinLED.cpp
    ProgressDialog.cpp
    QCommon.cpp
    StyledLabel.cpp
    TableCheckBox.cpp
    TableComboBox.cpp
)

# Add headers
target_sources(${TARGET_NAME} PRIVATE
    AboutDialog.h
    AlpacaApplication.h
    ApplicationEnhancements.h
    ColorSwatch.h
    CustomComboBox.h
    HappinessDialog.h
    IntFilteredEditor.h
    LEDWidget.h
    PainterEffects.h
    PinLED.h
    ProgressDialog.h
    QCommon.h
    QCommonGlobal.h
    StyledLabel.h
    TableCheckBox.h
    TableComboBox.h
)

# Add resources if they exist - integrate directly into the library
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/QCommon.qrc")
    target_sources(${TARGET_NAME} PRIVATE QCommon.qrc)
endif()

# UI files will be processed automatically by AUTOUIC
# No need for explicit qt6_add_ui() call when AUTOUIC is enabled

# Enable Qt MOC processing for Qt classes
set_target_properties(${TARGET_NAME} PROPERTIES
    OUTPUT_NAME ${TARGET_NAME}
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
)

# Add compile definitions
target_compile_definitions(${TARGET_NAME} PRIVATE
    QCOMMON_LIBRARY
)

# Link Qt libraries
target_link_libraries(${TARGET_NAME} PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Xml
    Qt6::SerialPort
    Qt6::Concurrent
    Qt6::Network
)

# Link to qcommon-console library
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(${TARGET_NAME} PUBLIC QCommon-consoled)
else()
    target_link_libraries(${TARGET_NAME} PUBLIC QCommon-console)
endif()

# Include directories - PUBLIC so dependent targets inherit them
target_include_directories(${TARGET_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/qcommon>
)

# Export target for use by other parts of the project
set_target_properties(${TARGET_NAME} PROPERTIES EXPORT_NAME QCommon)
