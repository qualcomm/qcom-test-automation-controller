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
find_package(Qt6 REQUIRED COMPONENTS Core Gui Xml SerialPort Concurrent Network)

# Define the library target
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(TARGET_NAME QCommon-consoled)
else()
    set(TARGET_NAME QCommon-console)
endif()

add_library(${TARGET_NAME} STATIC
    # Core files
    AlpacaDefines.cpp
    AlpacaDevice.cpp
    AlpacaScript.cpp
    AlpacaSettings.cpp
    AlpacaSharedLibrary.cpp
    AppCore.cpp
    BasicColor.cpp
    Button.cpp
    CharBit.cpp
    CommandLineParser.cpp
    ConsoleApplicationEnhancements.cpp
    DebugBoardType.cpp
    DriveThread.cpp
    EncryptedString.cpp
    FTDIChipset.cpp
    FTDIDevice.cpp
    FTDIPlatformConfiguration.cpp
    FrameCoder.cpp
    LicenseFailEvent.cpp
    PlatformConfiguration.cpp
    PlatformConfigurationException.cpp
    PlatformID.cpp
    PreferencesBase.cpp
    ProtocolInterface.cpp
    QCommonConsole.cpp
    QualcommChartColor.cpp
    QuitAppEvent.cpp
    RecentFiles.cpp
    SMTPClient.cpp
    ScriptVariable.cpp
    SerialPort.cpp
    StringProof.cpp
    StringUtilities.cpp
    SystemInformationWin.cpp
    SystemUsername.cpp
    TACCommand.cpp
    TACCommands.cpp
    TACPreferences.cpp
    Tabs.cpp
    ThreadedLog.cpp
    TickCount.cpp
    USBDescriptors.cpp
    mymemcpy.cpp
    
    # Private implementation files
    private/TACDriveThread.cpp
    private/TACLiteCoder.cpp
    private/TACLiteCommand.cpp
    private/TACLiteDriveThread.cpp
    private/TACLiteProtocol.cpp
    
    # Additional source files found in directory
    CommListModel.cpp
    CountingStream.cpp
    CSV.cpp
    KeyState.cpp
    ProcessUtilities.cpp

)

# Add headers - only include existing files
set(HEADER_FILES
    AlpacaDefines.h
    AlpacaDevice.h
    AlpacaScript.h
    AlpacaSettings.h
    AlpacaSharedLibrary.h
    AppCore.h
    BasicColor.h
    Button.h
    ByteCountFormatter.h
    CharBit.h
    CommandGroup.h
    CommandLineParser.h
    CommDevice.h
    CommListModel.h
    ConsoleApplicationEnhancements.h
    ConsoleInterface.h
    CountingStream.h
    CSV.h
    DataPoints.h
    DateCheckFailEvent.h
    DebugBoardType.h
    DriveThread.h
    EncryptedString.h
    ErrorParameter.h
    FrameCoder.h
    FramePackage.h
    ftd2xx.h
    FTDIChipset.h
    FTDIDevice.h
    FTDIPinSet.h
    FTDIPlatformConfiguration.h
    Hexify.h
    KeyState.h
    KingFisherCoder.h
    LicenseFailEvent.h
    mymemcpy.h
    Notification.h
    PinEntry.h
    PinID.h
    PlatformConfiguration.h
    PlatformConfigurationException.h
    PlatformID.h
    PreferencesBase.h
    PrintEfEr.h
    ProcessUtilities.h
    ProtocolInterface.h
    QCommonConsole.h
    QCommonConsoleGlobal.h
    QShrinkFile.h
    QualcommChartColor.h
    QualcommColors.h
    QuitAppEvent.h
    Range.h
    RangedContainer.h
    ReceiveInterface.h
    RecentFiles.h
    ScriptVariable.h
    SendInterface.h
    SerialPort.h
    SMTPClient.h
    StringProof.h
    StringUtilities.h
    SystemInformation.h
    SystemUsername.h
    Tabs.h
    TACCommand.h
    TacCommandHashes.h
    TACCommands.h
    TACDefines.h
    TacException.h
    TACPreferences.h
    ThreadedLog.h
    Throttle.h
    TickCount.h
    USBDescriptors.h
    version.h
    WinTypes.h
    private/TACDriveThread.h
    private/TACLiteCoder.h
    private/TACLiteCommand.h
    private/TACLiteDriveThread.h
    private/TACLiteProtocol.h
)

# Filter headers to only include existing ones
set(EXISTING_HEADERS)
foreach(header_file ${HEADER_FILES})
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${header_file}")
        list(APPEND EXISTING_HEADERS ${header_file})
    endif()
endforeach()

# Add existing headers to target
if(EXISTING_HEADERS)
    target_sources(${TARGET_NAME} PRIVATE ${EXISTING_HEADERS})
endif()

# Add resources directly to the target sources
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/resources/qcommon-console.qrc")
    target_sources(${TARGET_NAME} PRIVATE resources/qcommon-console.qrc)
endif()

# Enable Qt MOC processing for Qt classes
set_target_properties(${TARGET_NAME} PROPERTIES
    OUTPUT_NAME ${TARGET_NAME}
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
    AUTOMOC ON
    AUTORCC ON
)

# Add compile definitions
target_compile_definitions(${TARGET_NAME} PRIVATE
    QCOMMONCONSOLE_LIBRARY
)

# Link Qt libraries
target_link_libraries(${TARGET_NAME} PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Xml
    Qt6::SerialPort
    Qt6::Concurrent
    Qt6::Network
)

# Link FTDI library if found
if(FTDI_FOUND)
    target_link_libraries(${TARGET_NAME} PUBLIC FTDI::ftd2xx)
endif()

# Include directories - PUBLIC so dependent targets inherit them
target_include_directories(${TARGET_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/private>
    $<INSTALL_INTERFACE:include/qcommon-console>
)

# Platform-specific configurations
if(WIN32)
    target_compile_definitions(${TARGET_NAME} PRIVATE FTD2XX_STATIC)
elseif(UNIX AND NOT APPLE)
    # Linux-specific include paths would be added here if needed
endif()

# Export target for use by other parts of the project
set_target_properties(${TARGET_NAME} PROPERTIES EXPORT_NAME QCommonConsole)
