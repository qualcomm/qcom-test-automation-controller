# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries. 
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted (subject to the limitations in the
# disclaimer below) provided that the following conditions are met:
#     
#     * Redistributions of source code must retain the above copyright
#         notice, this list of conditions and the following disclaimer.
#     
#     * Redistributions in binary form must reproduce the above
#         copyright notice, this list of conditions and the following
#         disclaimer in the documentation and/or other materials provided
#         with the distribution.
#     
#     * Neither the name of Qualcomm Technologies, Inc. nor the names of its
#         contributors may be used to endorse or promote products derived
#         from this software without specific prior written permission.
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

# Author: Biswajit Roy (biswroy@qti.qualcomm.com)

include(${CMAKE_CURRENT_SOURCE_DIR}/Common.cmake)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(LIBRARY_NAME QCommonConsoled)
else()
    set(LIBRARY_NAME QCommonConsole)
endif()

qt_add_resources(QCOMMONCONSOLE_RESOURCES
    resources/QCCResource.qrc
)

qt_add_library(${LIBRARY_NAME} STATIC
    AlpacaDefines.cpp AlpacaDefines.h
    AlpacaDevice.cpp AlpacaDevice.h
    AlpacaScript.cpp AlpacaScript.h
    AlpacaSettings.cpp AlpacaSettings.h
    AlpacaSharedLibrary.cpp AlpacaSharedLibrary.h
    AppCore.cpp AppCore.h
    BasicColor.cpp BasicColor.h
    Button.cpp Button.h
    CharBit.cpp CharBit.h
    CommandGroup.h
    CommandLineParser.cpp CommandLineParser.h
    ConsoleApplicationEnhancements.cpp ConsoleApplicationEnhancements.h
    ConsoleInterface.h
    DateCheckFailEvent.h
    DebugBoardType.cpp DebugBoardType.h
    DriveThread.cpp DriveThread.h
    EncryptedString.cpp EncryptedString.h
    ErrorParameter.h
    FTDIChipset.cpp FTDIChipset.h
    FTDIDevice.cpp FTDIDevice.h
    FTDIPinSet.h
    FTDIPlatformConfiguration.cpp FTDIPlatformConfiguration.h
    FrameCoder.cpp FrameCoder.h
    FramePackage.h
    LicenseFailEvent.cpp LicenseFailEvent.h
    Notification.h
    PinID.h
    PlatformConfiguration.cpp PlatformConfiguration.h
    PlatformConfigurationException.cpp PlatformConfigurationException.h
    PlatformID.cpp PlatformID.h
    PreferencesBase.cpp PreferencesBase.h
    ProtocolInterface.cpp ProtocolInterface.h
    QCommonConsole.cpp QCommonConsole.h
    QCommonConsoleGlobal.h
    QualcommChartColor.cpp QualcommChartColor.h
    QualcommColors.h
    QuitAppEvent.cpp QuitAppEvent.h
    Range.h
    RangedContainer.h
    ReceiveInterface.h
    RecentFiles.cpp RecentFiles.h
    SMTPClient.cpp SMTPClient.h
    ScriptVariable.cpp ScriptVariable.h
    SendInterface.h
    SerialPort.cpp SerialPort.h
    StringProof.cpp StringProof.h
    StringUtilities.cpp StringUtilities.h
    SystemInformation.h
    SystemInformationWin.cpp
    SystemUsername.cpp SystemUsername.h
    TACCommand.cpp TACCommand.h
    TACCommands.cpp TACCommands.h
    TACPreferences.cpp TACPreferences.h
    Tabs.cpp Tabs.h
    TacCommandHashes.h
    TacException.h
    ThreadedLog.cpp ThreadedLog.h
    Throttle.h
    TickCount.cpp TickCount.h
    USBDescriptors.cpp USBDescriptors.h
    mymemcpy.cpp mymemcpy.h
    private/TACDriveThread.cpp private/TACDriveThread.h
    private/TACLiteCoder.cpp private/TACLiteCoder.h
    private/TACLiteCommand.cpp private/TACLiteCommand.h
    private/TACLiteDriveThread.cpp private/TACLiteDriveThread.h
    private/TACLiteProtocol.cpp private/TACLiteProtocol.h
    version.h

    ${QCOMMONCONSOLE_RESOURCES}
)

set_target_properties(${LIBRARY_NAME} PROPERTIES
    VERSION ALPACA_VERSION
)

QCommonConsoleSettings(${LIBRARY_NAME})

target_compile_definitions(${LIBRARY_NAME} PUBLIC
    QCOMMONCONSOLE_LIBRARY
    QTAC_STATIC
    QT_DISABLE_DEPRECATED_UP_TO=0x060600
    $<$<CONFIG:Debug>:DEBUG;_DEBUG>
    $<$<NOT:$<CONFIG:Debug>>:NDEBUG;_NDEBUG>
    $<$<PLATFORM_ID:Windows>:FTD2XX_STATIC>
)

target_link_libraries(${LIBRARY_NAME} PUBLIC
    Qt6::Concurrent
    Qt6::Core
    Qt6::Network
    Qt6::SerialPort
    Qt6::Xml
    ftd2xx
)
