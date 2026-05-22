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

cmake_minimum_required(VERSION 3.16)
project(qtac-core VERSION 0.1.0 LANGUAGES CXX)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/QtacCommon.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(qtac-core STATIC
    src/qtac_core.cpp
    src/String.cpp
    src/StringList.cpp
    src/Variant.cpp
    # FTDI stack (Qt-free)
    src/StringUtilities.cpp
    src/AlpacaDevice.cpp
    src/FTDIPlatformConfiguration.cpp
    src/FTDIChipset.cpp
    src/FTDIDevice.cpp
)

target_include_directories(qtac-core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(qtac-core PUBLIC cxx_std_20)
QtacCoreSettings(qtac-core)

# -----------------------------------------------------------------------
# FTDI D2XX library
# Assumes the ftd2xx SDK is installed in one of the standard locations.
# Adjust paths as needed for your environment.
# -----------------------------------------------------------------------
option(QTAC_BUILD_FTDI "Include FTDI hardware support in qtac-core" ON)

if(QTAC_BUILD_FTDI)
    if(WIN32)
        # Windows: link ftd2xx.lib from the FTDI CDM driver package
        find_library(FTD2XX_LIB ftd2xx
            HINTS
                "$ENV{PROGRAMFILES}/FTDI/CDM Driver Package - Bus D2XX (v2.12.28)/Static/amd64"
                "${CMAKE_CURRENT_SOURCE_DIR}/../../third-party/ftd2xx/win64"
        )
        if(FTD2XX_LIB)
            target_link_libraries(qtac-core PUBLIC ${FTD2XX_LIB})
        else()
            message(WARNING "ftd2xx library not found; FTDI hardware support disabled")
        endif()
    elseif(UNIX)
        # Linux: link libftd2xx.a or libftd2xx.so
        find_library(FTD2XX_LIB ftd2xx
            HINTS
                "/usr/local/lib"
                "${CMAKE_CURRENT_SOURCE_DIR}/../../third-party/ftd2xx/linux"
        )
        if(FTD2XX_LIB)
            target_link_libraries(qtac-core PUBLIC ${FTD2XX_LIB} dl pthread)
        else()
            message(WARNING "ftd2xx library not found; FTDI hardware support disabled")
        endif()
    endif()

    # ftd2xx.h location — expected alongside this CMakeLists.txt or in the
    # qcommon-console source tree that is a sibling of source/
    target_include_directories(qtac-core PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../src/libraries/qcommon-console
    )
endif()
