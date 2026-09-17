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

# Platform-specific settings
set(QTAC_CORE_DEFINITIONS QTAC_STATIC)

if(WIN32)
    list(APPEND QTAC_CORE_DEFINITIONS _WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /INCREMENTAL:NO")
endif()

if(UNIX)
    list(APPEND QTAC_CORE_DEFINITIONS __X86_64__)

    set(QTAC_CORE_CXX_FLAGS
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
        -Wno-pragmas
        -fPIC
    )

    set(QTAC_CORE_LINK_FLAGS
        -Wl,--rpath=$ORIGIN
        -Wl,--rpath=$ORIGIN/lib
        -Wl,--rpath=$ORIGIN/../lib
        -fPIC
    )
endif()

# Architecture-specific definitions
if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(STATUS "qtac-core: Building 32 bit")
    if(WIN32)
        list(APPEND QTAC_CORE_DEFINITIONS _WIN32)
    elseif(UNIX)
        list(APPEND QTAC_CORE_DEFINITIONS __i386__)
    endif()
    set(QTAC_WINTARGET "Win32")
else()
    message(STATUS "qtac-core: Building 64 bit")
    if(UNIX)
        list(APPEND QTAC_CORE_DEFINITIONS __X86_64__)
    endif()
    set(QTAC_WINTARGET "x64")
endif()

# Build configuration
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    list(APPEND QTAC_CORE_DEFINITIONS _DEBUG DEBUG)
    if(WIN32)
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi")
        set(QTAC_CONFIGURATION "${QTAC_WINTARGET}/Debug")
    else()
        set(QTAC_CONFIGURATION "Linux/Debug")
    endif()
else()
    list(APPEND QTAC_CORE_DEFINITIONS _NDEBUG NDEBUG)
    if(WIN32)
        set(QTAC_CONFIGURATION "${QTAC_WINTARGET}/Release")
    else()
        set(QTAC_CONFIGURATION "Linux/Release")
    endif()
endif()

# Build paths
set(QTAC_BUILDROOT ${CMAKE_SOURCE_DIR}/__Builds/${QTAC_CONFIGURATION})
set(QTAC_BINPATH ${QTAC_BUILDROOT}/bin)
set(QTAC_STATIC_LIBPATH ${QTAC_BUILDROOT}/lib)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${QTAC_BINPATH})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${QTAC_STATIC_LIBPATH})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${QTAC_STATIC_LIBPATH})

# Function to apply common settings to a target
function(QtacCoreSettings target_name)
    target_compile_definitions(${target_name} PRIVATE ${QTAC_CORE_DEFINITIONS})

    if(UNIX AND QTAC_CORE_CXX_FLAGS)
        target_compile_options(${target_name} PRIVATE ${QTAC_CORE_CXX_FLAGS})
    endif()

    if(UNIX AND QTAC_CORE_LINK_FLAGS)
        target_link_options(${target_name} PRIVATE ${QTAC_CORE_LINK_FLAGS})
    endif()
endfunction()
