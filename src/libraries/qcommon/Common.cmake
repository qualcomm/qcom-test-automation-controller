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

include(${CMAKE_SOURCE_DIR}/src/libraries/qcommon-console/version.cmake)

# Set path variables
set(QCOMMON ${CMAKE_SOURCE_DIR}/src/libraries/qcommon)
set(QCOMMONCONSOLE ${CMAKE_SOURCE_DIR}/src/libraries/qcommon-console)
set(LIBUI ${CMAKE_SOURCE_DIR}/src/libraries/ui-common)
set(TACDEV ${CMAKE_SOURCE_DIR}/interfaces/C++/TACDev)
set(TACLOC ${CMAKE_SOURCE_DIR}/src/applications/TAC)

# Common compile definitions
set(QCOMMON_DEFINITIONS
    QT_DEPRECATED_WARNINGS
    QT_DISABLE_DEPRECATED_BEFORE=0x060000
    QTAC_STATIC
)

# Platform-specific settings
if(WIN32)
    set(CMAKE_CXX_STANDARD 20)
    list(APPEND QCOMMON_DEFINITIONS _WIN32)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi")
        set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /INCREMENTAL:NO")
    endif()
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /INCREMENTAL:NO")
endif()

if(UNIX)
    set(CMAKE_CXX_STANDARD 20)
    list(APPEND QCOMMON_DEFINITIONS __X86_64__)
    
    # Unix compiler flags
    set(QCOMMON_CXX_FLAGS
        -Werror
        -Wno-unused-result
        -Wno-write-strings
        -Wno-comment
        -Wno-unused-function
        -Wno-reorder
        -Wno-unknown-pragmas
        -Wno-conversion-null
        -Wno-unused-parameter
        -Wno-overloaded-virtual
        -Wno-unused-variable
        -Wno-date-time
    )
endif()

# Architecture-specific definitions
if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(STATUS "Building 32 bit")
    if(WIN32)
        list(APPEND QCOMMON_DEFINITIONS _WIN32)
    elseif(UNIX)
        list(APPEND QCOMMON_DEFINITIONS __i386__)
    endif()
    set(WINTARGET "Win32")
else()
    message(STATUS "Building 64 bit")
    if(UNIX)
        list(APPEND QCOMMON_DEFINITIONS __X86_64__)
    endif()
    set(WINTARGET "x64")
endif()

# Build configuration
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    list(APPEND QCOMMON_DEFINITIONS _DEBUG DEBUG)
    if(WIN32)
        set(CONFIGURATION "${WINTARGET}/Debug")
    else()
        set(CONFIGURATION "Linux/Debug")
    endif()
else()
    list(APPEND QCOMMON_DEFINITIONS _NDEBUG NDEBUG)
    if(WIN32)
        set(CONFIGURATION "${WINTARGET}/Release")
    else()
        set(CONFIGURATION "Linux/Release")
    endif()
endif()

# Build paths
set(BUILDROOT ${CMAKE_SOURCE_DIR}/__Builds/${CONFIGURATION})
set(BINPATH ${BUILDROOT}/bin)
set(STATIC_LIBPATH ${BUILDROOT}/lib)

# Set CMake output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BINPATH})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${STATIC_LIBPATH})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${STATIC_LIBPATH})

# Function to apply common settings to a target
function(QCommonSettings target_name)
    target_compile_definitions(${target_name} PRIVATE ${QCOMMON_DEFINITIONS})
    
    if(UNIX AND QCOMMON_CXX_FLAGS)
        target_compile_options(${target_name} PRIVATE ${QCOMMON_CXX_FLAGS})
    endif()
    
    target_include_directories(${target_name} PRIVATE ${BUILDROOT}/ui/QCommon)
endfunction()
