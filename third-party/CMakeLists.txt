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

cmake_minimum_required(VERSION 3.16)

# Primary: GitHub Release mirror (not behind Cloudflare, works from any machine).
# Fallback: ftdichip.com direct (blocked by Cloudflare on some networks).
set(FTDI_GITHUB_RELEASE_BASE "https://github.com/qualcomm/qcom-test-automation-controller/releases/download/v0.1.0-deps")

if(WIN32)
    set(FTDI_ARCHIVE "CDM-v2.12.36.20-WHQL-Certified.zip")
    set(FTDI_URL_PRIMARY "${FTDI_GITHUB_RELEASE_BASE}/${FTDI_ARCHIVE}")
    set(FTDI_URL_FALLBACK "https://ftdichip.com/wp-content/uploads/2025/03/CDM-v2.12.36.20-WHQL-Certified.zip")
    set(DEBUG_LIB "${CMAKE_SOURCE_DIR}/__Builds/x64/Debug/lib/ftd2xx.lib")
    set(RELEASE_LIB "${CMAKE_SOURCE_DIR}/__Builds/x64/Release/lib/ftd2xx.lib")
else()
    set(FTDI_ARCHIVE "libftd2xx-linux-x86_64-1.4.33.tgz")
    set(FTDI_URL_PRIMARY "${FTDI_GITHUB_RELEASE_BASE}/${FTDI_ARCHIVE}")
    set(FTDI_URL_FALLBACK "https://ftdichip.com/wp-content/uploads/2025/03/libftd2xx-linux-x86_64-1.4.33.tgz")
    set(DEBUG_LIB "${CMAKE_SOURCE_DIR}/__Builds/Linux/Debug/lib/libftd2xx.a")
    set(RELEASE_LIB "${CMAKE_SOURCE_DIR}/__Builds/Linux/Release/lib/libftd2xx.a")
endif()

set(ARCHIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${FTDI_ARCHIVE}")
set(TEMP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/temp_extract")

# Allow specifying a pre-downloaded archive to bypass the download step entirely.
# Useful in air-gapped or fully restricted environments.
# Usage: cmake -DFTDI_ARCHIVE_PATH=/path/to/<archive> ...
set(FTDI_ARCHIVE_PATH "" CACHE FILEPATH "Path to a pre-downloaded FTDI archive (skips download)")

set(NEED_DOWNLOAD FALSE)
if(NOT EXISTS "${DEBUG_LIB}" OR NOT EXISTS "${RELEASE_LIB}")
    set(NEED_DOWNLOAD TRUE)
endif()

# Try to download a URL and validate it is a real archive (not a Cloudflare block page).
# Sets FTDI_DOWNLOAD_OK to TRUE on success.
macro(try_ftdi_download URL)
    set(FTDI_DOWNLOAD_OK FALSE)
    set(RETRY_COUNT 0)
    while(RETRY_COUNT LESS 3)
        math(EXPR RETRY_COUNT "${RETRY_COUNT} + 1")
        if(EXISTS "${ARCHIVE_PATH}")
            file(REMOVE "${ARCHIVE_PATH}")
        endif()
        file(DOWNLOAD "${URL}" "${ARCHIVE_PATH}" SHOW_PROGRESS STATUS DOWNLOAD_STATUS TIMEOUT 300)
        list(GET DOWNLOAD_STATUS 0 DOWNLOAD_ERROR_CODE)
        if(DOWNLOAD_ERROR_CODE EQUAL 0 AND EXISTS "${ARCHIVE_PATH}")
            execute_process(COMMAND ${CMAKE_COMMAND} -E tar tf "${ARCHIVE_PATH}" RESULT_VARIABLE TAR_RESULT OUTPUT_QUIET ERROR_QUIET)
            if(TAR_RESULT EQUAL 0)
                set(FTDI_DOWNLOAD_OK TRUE)
                break()
            endif()
        endif()
        if(RETRY_COUNT LESS 3)
            execute_process(COMMAND ${CMAKE_COMMAND} -E sleep 5)
        endif()
    endwhile()
endmacro()

if(NEED_DOWNLOAD)
    if(FTDI_ARCHIVE_PATH AND EXISTS "${FTDI_ARCHIVE_PATH}")
        message(STATUS "Using pre-downloaded FTDI archive: ${FTDI_ARCHIVE_PATH}")
        file(COPY_FILE "${FTDI_ARCHIVE_PATH}" "${ARCHIVE_PATH}")
    else()
        # Try GitHub Release mirror first, then fall back to ftdichip.com.
        message(STATUS "Downloading FTDI library from GitHub Release mirror...")
        try_ftdi_download("${FTDI_URL_PRIMARY}")
        if(NOT FTDI_DOWNLOAD_OK)
            message(STATUS "GitHub Release mirror failed, trying ftdichip.com directly...")
            try_ftdi_download("${FTDI_URL_FALLBACK}")
        endif()
        if(NOT FTDI_DOWNLOAD_OK)
            message(FATAL_ERROR "Failed to download FTDI library from all sources.\n"
                "The ftdichip.com URL may be blocked by Cloudflare on this network.\n"
                "To fix: manually download ${FTDI_ARCHIVE} from a browser, then re-run CMake with:\n"
                "  -DFTDI_ARCHIVE_PATH=/path/to/${FTDI_ARCHIVE}")
        endif()
    endif()
    
    file(REMOVE_RECURSE "${TEMP_DIR}")
    file(MAKE_DIRECTORY "${TEMP_DIR}")
    execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf "${ARCHIVE_PATH}" WORKING_DIRECTORY "${TEMP_DIR}")
    
    if(WIN32)
        file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/__Builds/x64/Debug/lib")
        file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/__Builds/x64/Release/lib")
        file(GLOB SOURCE_LIB "${TEMP_DIR}/amd64/ftd2xx.lib")

        file(GLOB SOURCE_DLL "${TEMP_DIR}/amd64/FTD2XX64.dll")
        if(SOURCE_DLL)
            file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/__Builds/x64/Debug/bin")
            file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/__Builds/x64/Release/bin")
            file(COPY "${SOURCE_DLL}" DESTINATION "${CMAKE_SOURCE_DIR}/__Builds/x64/Debug/bin")
            file(RENAME "${CMAKE_SOURCE_DIR}/__Builds/x64/Debug/bin/FTD2XX64.dll" "${CMAKE_SOURCE_DIR}/__Builds/x64/Debug/bin/ftd2xx.dll")
            file(COPY "${SOURCE_DLL}" DESTINATION "${CMAKE_SOURCE_DIR}/__Builds/x64/Release/bin")
            file(RENAME "${CMAKE_SOURCE_DIR}/__Builds/x64/Release/bin/FTD2XX64.dll" "${CMAKE_SOURCE_DIR}/__Builds/x64/Release/bin/ftd2xx.dll")
        endif()

        if(NOT SOURCE_LIB)
            file(GLOB SOURCE_LIB "${TEMP_DIR}/Static/amd64/ftd2xx.lib")
        endif()
        file(COPY "${SOURCE_LIB}" DESTINATION "${CMAKE_SOURCE_DIR}/__Builds/x64/Debug/lib")
        file(COPY "${SOURCE_LIB}" DESTINATION "${CMAKE_SOURCE_DIR}/__Builds/x64/Release/lib")
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
    set_target_properties(ftd2xx PROPERTIES IMPORTED_LOCATION "${DEBUG_LIB}")
    set_target_properties(ftd2xx PROPERTIES IMPORTED_LOCATION "${RELEASE_LIB}")
endif()

if(WIN32 AND DEFINED ENV{QTDIR})
    set(QT_BIN_DIR "$ENV{QTDIR}/bin")
    set(QT_PLUGINS_DIR "$ENV{QTDIR}/plugins")
    set(DEBUG_BIN_DIR "${CMAKE_SOURCE_DIR}/__Builds/x64/Debug/bin")
    set(RELEASE_BIN_DIR "${CMAKE_SOURCE_DIR}/__Builds/x64/Release/bin")
    
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
