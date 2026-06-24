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

cmake_minimum_required(VERSION 3.22)

if(WIN32)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "ARM64|aarch64" OR CMAKE_GENERATOR_PLATFORM STREQUAL "ARM64")
        set(FTDI_URL "https://web.archive.org/web/20250821211500/https://ftdichip.com/wp-content/uploads/2025/03/CDM-v2.12.36.20-for-ARM64-WHQL-Certified.zip")
        set(FTDI_ARCHIVE "CDM-v2.12.36.20-for-ARM64-WHQL-Certified.zip")
        set(FTDI_WIN_ARCH "ARM64")
        set(FTDI_ZIP_LIB_GLOB "ARM64/Release/FTD2XXstatic.lib")
        set(FTDI_ZIP_DLL_GLOB "ARM64/Release/FTD2XX.dll")
    else()
        set(FTDI_URL "https://web.archive.org/web/20250820134143/https://ftdichip.com/wp-content/uploads/2023/09/CDM-v2.12.36.4-WHQL-Certified.zip")
        set(FTDI_ARCHIVE "CDM-v2.12.36.4-WHQL-Certified.zip")
        set(FTDI_WIN_ARCH "x64")
        set(FTDI_ZIP_LIB_GLOB "amd64/ftd2xx.lib")
        set(FTDI_ZIP_DLL_GLOB "amd64/FTD2XX64.dll")
    endif()

    set(DEBUG_LIB "${CMAKE_SOURCE_DIR}/__Builds/${FTDI_WIN_ARCH}/Debug/lib/ftd2xx.lib")
    set(RELEASE_LIB "${CMAKE_SOURCE_DIR}/__Builds/${FTDI_WIN_ARCH}/Release/lib/ftd2xx.lib")
else()
    set(FTDI_URL "https://web.archive.org/web/20250822044524/https://ftdichip.com/wp-content/uploads/2025/03/libftd2xx-linux-x86_64-1.4.33.tgz")
    set(FTDI_ARCHIVE "libftd2xx-linux-x86_64-1.4.33.tgz")
    set(DEBUG_LIB "${CMAKE_SOURCE_DIR}/__Builds/Linux/Debug/lib/libftd2xx.a")
    set(RELEASE_LIB "${CMAKE_SOURCE_DIR}/__Builds/Linux/Release/lib/libftd2xx.a")
endif()

set(ARCHIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${FTDI_ARCHIVE}")
set(TEMP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/temp_extract")

set(NEED_DOWNLOAD FALSE)
if(NOT EXISTS "${DEBUG_LIB}" OR NOT EXISTS "${RELEASE_LIB}")
    set(NEED_DOWNLOAD TRUE)
endif()

if(NEED_DOWNLOAD)
    set(RETRY_COUNT 0)
    set(DOWNLOAD_SUCCESS FALSE)
    while(RETRY_COUNT LESS 3)
        math(EXPR RETRY_COUNT "${RETRY_COUNT} + 1")
        if(EXISTS "${ARCHIVE_PATH}")
            file(REMOVE "${ARCHIVE_PATH}")
        endif()
        file(DOWNLOAD "${FTDI_URL}" "${ARCHIVE_PATH}" SHOW_PROGRESS STATUS DOWNLOAD_STATUS TIMEOUT 300)
        list(GET DOWNLOAD_STATUS 0 DOWNLOAD_ERROR_CODE)
        if(DOWNLOAD_ERROR_CODE EQUAL 0 AND EXISTS "${ARCHIVE_PATH}")
            execute_process(COMMAND ${CMAKE_COMMAND} -E tar tf "${ARCHIVE_PATH}" RESULT_VARIABLE TAR_RESULT OUTPUT_QUIET ERROR_QUIET)
            if(TAR_RESULT EQUAL 0)
                set(DOWNLOAD_SUCCESS TRUE)
                break()
            endif()
        endif()
        if(RETRY_COUNT LESS 3)
            execute_process(COMMAND ${CMAKE_COMMAND} -E sleep 5)
        endif()
    endwhile()
    if(NOT DOWNLOAD_SUCCESS)
        message(FATAL_ERROR
            "Failed to download the FTDI library archive after ${RETRY_COUNT} attempts.\n"
            "Manually download '${FTDI_ARCHIVE}' from:\n"
            "  ${FTDI_URL}\n"
            "Place the downloaded file at:\n"
            "  ${ARCHIVE_PATH}\n"
            "Then re-run cmake.")
    endif()
    
    file(REMOVE_RECURSE "${TEMP_DIR}")
    file(MAKE_DIRECTORY "${TEMP_DIR}")
    execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf "${ARCHIVE_PATH}" WORKING_DIRECTORY "${TEMP_DIR}")
    
    if(WIN32)
        file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/__Builds/${FTDI_WIN_ARCH}/Debug/lib")
        file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/__Builds/${FTDI_WIN_ARCH}/Release/lib")
        file(GLOB SOURCE_LIB "${TEMP_DIR}/${FTDI_ZIP_LIB_GLOB}")

        file(GLOB SOURCE_DLL "${TEMP_DIR}/${FTDI_ZIP_DLL_GLOB}")
        if(SOURCE_DLL)
            file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/__Builds/${FTDI_WIN_ARCH}/Debug/bin")
            file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/__Builds/${FTDI_WIN_ARCH}/Release/bin")
            file(COPY "${SOURCE_DLL}" DESTINATION "${CMAKE_SOURCE_DIR}/__Builds/${FTDI_WIN_ARCH}/Debug/bin")
            get_filename_component(SOURCE_DLL_NAME "${SOURCE_DLL}" NAME)
            if(NOT SOURCE_DLL_NAME STREQUAL "ftd2xx.dll")
                file(RENAME "${CMAKE_SOURCE_DIR}/__Builds/${FTDI_WIN_ARCH}/Debug/bin/${SOURCE_DLL_NAME}" "${CMAKE_SOURCE_DIR}/__Builds/${FTDI_WIN_ARCH}/Debug/bin/ftd2xx.dll")
            endif()
            file(COPY "${SOURCE_DLL}" DESTINATION "${CMAKE_SOURCE_DIR}/__Builds/${FTDI_WIN_ARCH}/Release/bin")
            if(NOT SOURCE_DLL_NAME STREQUAL "ftd2xx.dll")
                file(RENAME "${CMAKE_SOURCE_DIR}/__Builds/${FTDI_WIN_ARCH}/Release/bin/${SOURCE_DLL_NAME}" "${CMAKE_SOURCE_DIR}/__Builds/${FTDI_WIN_ARCH}/Release/bin/ftd2xx.dll")
            endif()
        endif()

        if(NOT SOURCE_LIB AND FTDI_WIN_ARCH STREQUAL "x64")
            file(GLOB SOURCE_LIB "${TEMP_DIR}/Static/amd64/ftd2xx.lib")
        endif()
        if(NOT SOURCE_LIB)
            message(FATAL_ERROR
                "FTDI ${FTDI_WIN_ARCH} lib not found in the downloaded CDM package.\n"
                "Manually download '${FTDI_ARCHIVE}' from:\n"
                "  ${FTDI_URL}\n"
                "Place the downloaded file at:\n"
                "  ${ARCHIVE_PATH}\n"
                "Then re-run cmake.")
        endif()
        file(COPY "${SOURCE_LIB}" DESTINATION "${CMAKE_SOURCE_DIR}/__Builds/${FTDI_WIN_ARCH}/Debug/lib")
        file(COPY "${SOURCE_LIB}" DESTINATION "${CMAKE_SOURCE_DIR}/__Builds/${FTDI_WIN_ARCH}/Release/lib")
        get_filename_component(SOURCE_LIB_NAME "${SOURCE_LIB}" NAME)
        if(NOT SOURCE_LIB_NAME STREQUAL "ftd2xx.lib")
            file(RENAME "${CMAKE_SOURCE_DIR}/__Builds/${FTDI_WIN_ARCH}/Debug/lib/${SOURCE_LIB_NAME}" "${DEBUG_LIB}")
            file(RENAME "${CMAKE_SOURCE_DIR}/__Builds/${FTDI_WIN_ARCH}/Release/lib/${SOURCE_LIB_NAME}" "${RELEASE_LIB}")
        endif()
    else()
        file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/__Builds/Linux/Debug/lib")
        file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/__Builds/Linux/Release/lib")
        file(GLOB SOURCE_LIB "${TEMP_DIR}/*/libftd2xx-static.a")
        file(COPY "${SOURCE_LIB}" DESTINATION "${CMAKE_SOURCE_DIR}/__Builds/Linux/Debug/lib")
        file(COPY "${SOURCE_LIB}" DESTINATION "${CMAKE_SOURCE_DIR}/__Builds/Linux/Release/lib")
        file(RENAME "${CMAKE_SOURCE_DIR}/__Builds/Linux/Debug/lib/libftd2xx-static.a" "${DEBUG_LIB}")
        file(RENAME "${CMAKE_SOURCE_DIR}/__Builds/Linux/Release/lib/libftd2xx-static.a" "${RELEASE_LIB}")
    endif()
    
    file(REMOVE_RECURSE "${TEMP_DIR}")
endif()

if(WIN32)
    add_library(ftd2xx_debug STATIC IMPORTED GLOBAL)
    add_library(ftd2xx_release STATIC IMPORTED GLOBAL)
    set_target_properties(ftd2xx_debug PROPERTIES IMPORTED_LOCATION "${DEBUG_LIB}")
    set_target_properties(ftd2xx_release PROPERTIES IMPORTED_LOCATION "${RELEASE_LIB}")
else()
    add_library(ftd2xx STATIC IMPORTED GLOBAL)
    set_target_properties(ftd2xx PROPERTIES
        IMPORTED_LOCATION_DEBUG   "${DEBUG_LIB}"
        IMPORTED_LOCATION_RELEASE "${RELEASE_LIB}"
        IMPORTED_LOCATION         "${RELEASE_LIB}"
    )
endif()

if(WIN32 AND DEFINED ENV{QTDIR})
    set(QT_BIN_DIR "$ENV{QTDIR}/bin")
    set(QT_PLUGINS_DIR "$ENV{QTDIR}/plugins")
    set(DEBUG_BIN_DIR "${CMAKE_SOURCE_DIR}/__Builds/${FTDI_WIN_ARCH}/Debug/bin")
    set(RELEASE_BIN_DIR "${CMAKE_SOURCE_DIR}/__Builds/${FTDI_WIN_ARCH}/Release/bin")
    
    set(QT_DLLS Qt6Core Qt6Gui Qt6Widgets Qt6SerialPort Qt6Concurrent Qt6Network Qt6Xml Qt6Multimedia Qt6MultimediaWidgets)
    
    file(MAKE_DIRECTORY "${DEBUG_BIN_DIR}")
    file(MAKE_DIRECTORY "${RELEASE_BIN_DIR}")
    file(MAKE_DIRECTORY "${DEBUG_BIN_DIR}/platforms")
    file(MAKE_DIRECTORY "${RELEASE_BIN_DIR}/platforms")
    
    foreach(QT_DLL ${QT_DLLS})
        if(EXISTS "${QT_BIN_DIR}/${QT_DLL}d.dll")
            file(COPY "${QT_BIN_DIR}/${QT_DLL}d.dll" DESTINATION "${DEBUG_BIN_DIR}")
        endif()
        if(EXISTS "${QT_BIN_DIR}/${QT_DLL}.dll")
            file(COPY "${QT_BIN_DIR}/${QT_DLL}.dll" DESTINATION "${RELEASE_BIN_DIR}")
        endif()
    endforeach()
    
    if(EXISTS "${QT_PLUGINS_DIR}/platforms/qwindowsd.dll")
        file(COPY "${QT_PLUGINS_DIR}/platforms/qwindowsd.dll" DESTINATION "${DEBUG_BIN_DIR}/platforms")
    endif()
    if(EXISTS "${QT_PLUGINS_DIR}/platforms/qwindows.dll")
        file(COPY "${QT_PLUGINS_DIR}/platforms/qwindows.dll" DESTINATION "${RELEASE_BIN_DIR}/platforms")
    endif()
endif()

function(link_ftd2xx target_name)
    if(WIN32)
        target_link_libraries(${target_name} PRIVATE $<$<CONFIG:Debug>:ftd2xx_debug> $<$<CONFIG:Release>:ftd2xx_release>)
    else()
        target_link_libraries(${target_name} PRIVATE ftd2xx pthread dl)
    endif()
endfunction()
