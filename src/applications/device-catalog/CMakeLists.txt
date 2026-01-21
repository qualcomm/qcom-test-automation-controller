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

include(${CMAKE_SOURCE_DIR}/src/libraries/qcommon/Common.cmake)

set(APP_NAME DeviceCatalog)

qt_add_resources(DC_RESOURCES
    resources/DeviceCatalog.qrc
)

qt_add_executable(${APP_NAME} WIN32
    DeviceCatalog.cpp DeviceCatalog.h DeviceCatalog.ui
    DeviceCatalogApplication.cpp DeviceCatalogApplication.h
    DeviceSelectionDialog.cpp DeviceSelectionDialog.h DeviceSelectionDialog.ui
    main.cpp

    ${DC_RESOURCES}
)

if(WIN32)
    set_target_properties(${APP_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
    target_sources(${APP_NAME} PRIVATE resources/DeviceCatalog.rc)
elseif(UNIX AND NOT APPLE)
    install(FILES resources/DeviceCatalog.desktop DESTINATION share/applications)
    install(FILES resources/device-catalog.png DESTINATION share/icons/hicolor/48x48/apps RENAME device-catalog.png)
endif()

target_include_directories(${APP_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src/libraries/qcommon-console
    ${CMAKE_SOURCE_DIR}/src/libraries/qcommon
    ${CMAKE_SOURCE_DIR}/src/libraries/ui-common
)

target_compile_definitions(${APP_NAME} PRIVATE
    QTAC_STATIC
    QT_DEPRECATED_WARNINGS
    QT_DISABLE_DEPRECATED_UP_TO=0x060600
    $<$<CONFIG:Debug>:DEBUG;_DEBUG>
    $<$<NOT:$<CONFIG:Debug>>:NDEBUG;_NDEBUG>
)

target_link_directories(${APP_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/__Builds/x64/$<CONFIG>/lib
)

target_link_libraries(${APP_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::SerialPort
    Qt6::Widgets
    ftd2xx
    $<$<CONFIG:Debug>:UId;QCommond;QCommonConsoled>
    $<$<NOT:$<CONFIG:Debug>>:UI;QCommon;QCommonConsole>
)

if(WIN32)
    target_link_libraries(${APP_NAME} PRIVATE
        Ole32
        SetupAPI
        Winusb
    )

    target_compile_options(${APP_NAME} PRIVATE
        $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:/Zi>
    )
endif()

if(LINUX)
    target_link_libraries(${APP_NAME} PRIVATE dl)
endif()

if(UNIX)
    target_compile_options(${APP_NAME} PRIVATE
        -Wno-comment
        -Wno-conversion-null
        -Wno-reorder
        -Wno-unknown-pragmas
        -Wno-unused-function
        -Wno-unused-result
        -Wno-write-strings
    )
endif()

install(TARGETS ${APP_NAME}
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET DeviceCatalog
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
