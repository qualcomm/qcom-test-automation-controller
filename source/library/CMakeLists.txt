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

# -----------------------------------------------------------------------
# libserialport — compiled from source for PSoC/PIC32CX serial communication.
# libserialport uses autotools, not CMake, so we fetch the sources and compile
# the relevant .c files directly (same approach as tac-core).
# -----------------------------------------------------------------------
include(FetchContent)
enable_language(C)

FetchContent_Declare(
    libserialport
    GIT_REPOSITORY https://github.com/sigrokproject/libserialport.git
    GIT_TAG        master
    GIT_SHALLOW    TRUE
)
FetchContent_GetProperties(libserialport)
if(NOT libserialport_POPULATED)
    FetchContent_Populate(libserialport)
endif()

set(LIBSERIALPORT_CONFIG_DIR "${CMAKE_CURRENT_BINARY_DIR}/libserialport_config")
file(MAKE_DIRECTORY "${LIBSERIALPORT_CONFIG_DIR}")

if(NOT WIN32)
    file(WRITE "${LIBSERIALPORT_CONFIG_DIR}/config.h" [=[
/* Generated config.h for libserialport on Linux/macOS */
#define SP_PRIV __attribute__((visibility("hidden")))
#define SP_API  __attribute__((visibility("default")))
#define HAVE_STRUCT_SERIAL_STRUCT 1
#define HAVE_STRUCT_TERMIOS2 1
#define HAVE_DECL_BOTHER 1
#define HAVE_REALPATH 1
#define HAVE_FLOCK 1
]=])
endif()

set(LIBSERIALPORT_SOURCES
    "${libserialport_SOURCE_DIR}/serialport.c"
    "${libserialport_SOURCE_DIR}/timing.c"
)
if(WIN32)
    list(APPEND LIBSERIALPORT_SOURCES "${libserialport_SOURCE_DIR}/windows.c")
else()
    list(APPEND LIBSERIALPORT_SOURCES
        "${libserialport_SOURCE_DIR}/linux.c"
        "${libserialport_SOURCE_DIR}/linux_termios.c"
    )
endif()

add_library(serialport STATIC ${LIBSERIALPORT_SOURCES})
target_include_directories(serialport PUBLIC "${libserialport_SOURCE_DIR}")
if(NOT WIN32)
    target_include_directories(serialport PRIVATE "${LIBSERIALPORT_CONFIG_DIR}")
endif()
target_compile_definitions(serialport
    PUBLIC  $<$<PLATFORM_ID:Windows>:LIBSERIALPORT_MSBUILD>
    PRIVATE $<$<PLATFORM_ID:Windows>:_WIN32>
    PRIVATE $<$<NOT:$<PLATFORM_ID:Windows>>:LIBSERIALPORT_ATBUILD>
)
if(WIN32)
    target_link_libraries(serialport PRIVATE setupapi)
else()
    target_link_libraries(serialport PRIVATE pthread)
endif()
if(MSVC)
    target_compile_options(serialport PRIVATE /W0)
else()
    target_compile_options(serialport PRIVATE -w)
endif()

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
    # Serial port (W2)
    src/SerialPort.cpp
    src/SerialPortInfo.cpp
    # Drive thread base (W2)
    src/DriveThread.cpp
    src/FrameCoder.cpp
    src/ProtocolInterface.cpp
    src/ThreadedLog.cpp
    src/TACDriveThread.cpp
    # TAC Lite (FTDI) drive thread (W2)
    src/TACLiteCommand.cpp
    src/TACLiteCoder.cpp
    src/TACLiteProtocol.cpp
    src/TACLiteDriveThread.cpp
    # TAC PSOC (serial) drive thread (W2)
    src/TACPSOCCommand.cpp
    src/TACPSOCCoder.cpp
    src/TACPSOCProtocol.cpp
    src/TACPSOCDriveThread.cpp
    # TAC PIC32CX (serial) drive thread (W2)
    src/TACPIC32CXCommand.cpp
    src/TACPIC32CXCoder.cpp
    src/TACPIC32CXProtocol.cpp
    src/TACPIC32CXDriveThread.cpp
    # PSoC/PIC32CX devices and platform configurations (W1)
    src/PSOCPlatformConfiguration.cpp
    src/PIC32CXPlatformConfiguration.cpp
    src/PSOCDevice.cpp
    src/PIC32CXDevice.cpp
    # Platform registry / devicelist.json loader (Phase 4)
    src/PlatformID.cpp
    src/TcnfLoader.cpp
)

# Platform-specific SerialPortInfo enumeration
if(WIN32)
    target_sources(qtac-core PRIVATE src/platform/SerialPortInfo_win32.cpp)
else()
    target_sources(qtac-core PRIVATE src/platform/SerialPortInfo_posix.cpp)
endif()

target_include_directories(qtac-core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(qtac-core PUBLIC cxx_std_20)
QtacCoreSettings(qtac-core)

target_link_libraries(qtac-core PUBLIC serialport)

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
