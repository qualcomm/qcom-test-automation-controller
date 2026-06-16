# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

# Include version information
include(${CMAKE_SOURCE_DIR}/src/libraries/qcommon-console/version.cmake)

# Set path variables
set(QCOMMONCONSOLE ${CMAKE_SOURCE_DIR}/src/libraries/qcommon-console)
set(TACDEV ${CMAKE_SOURCE_DIR}/interfaces/C++/TACDev)

# Common compile definitions
set(QCOMMONCONSOLE_DEFINITIONS
    QT_DISABLE_DEPRECATED_UP_TO=0x060900
    QTAC_STATIC
)

# Platform-specific settings
if(WIN32)
    set(CMAKE_CXX_STANDARD 20)
    list(APPEND QCOMMONCONSOLE_DEFINITIONS _WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /INCREMENTAL:NO")
endif()

if(UNIX)
    set(CMAKE_CXX_STANDARD 20)

    # Unix compiler flags
    set(QCOMMONCONSOLE_CXX_FLAGS
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
    
    # Unix linker flags
    set(QCOMMONCONSOLE_LINK_FLAGS
        -Wl,--rpath=$ORIGIN
        -Wl,--rpath=$ORIGIN/lib
        -Wl,--rpath=$ORIGIN/../lib
        -fPIC
    )
endif()

if(UNIX AND NOT APPLE)
    # Additional flags for Linux
    list(APPEND QCOMMONCONSOLE_LINK_FLAGS -Wl,--rpath=$ORIGIN -Wl,--rpath=$ORIGIN/../lib)
endif()

# Architecture-specific definitions
if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(STATUS "Building 32 bit")
    if(WIN32)
        list(APPEND QCOMMONCONSOLE_DEFINITIONS _WIN32)
    elseif(UNIX)
        list(APPEND QCOMMONCONSOLE_DEFINITIONS __i386__)
    endif()
    set(WINTARGET "Win32")
else()
    message(STATUS "Building 64 bit")
    if(UNIX)
        list(APPEND QCOMMONCONSOLE_DEFINITIONS __X86_64__)
    endif()
    set(WINTARGET "x64")
endif()

# Build configuration
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    list(APPEND QCOMMONCONSOLE_DEFINITIONS _DEBUG DEBUG)
    if(WIN32)
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi")
        set(CONFIGURATION "${WINTARGET}/Debug")
    else()
        set(CONFIGURATION "Linux/Debug")
    endif()
else()
    list(APPEND QCOMMONCONSOLE_DEFINITIONS _NDEBUG NDEBUG)
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
function(QCommonConsoleSettings target_name)
    target_compile_definitions(${target_name} PRIVATE ${QCOMMONCONSOLE_DEFINITIONS})
    
    if(UNIX AND QCOMMONCONSOLE_CXX_FLAGS)
        target_compile_options(${target_name} PRIVATE ${QCOMMONCONSOLE_CXX_FLAGS})
    endif()
    
    if(UNIX AND QCOMMONCONSOLE_LINK_FLAGS)
        target_link_options(${target_name} PRIVATE ${QCOMMONCONSOLE_LINK_FLAGS})
    endif()
    
    target_include_directories(${target_name} PRIVATE ${BUILDROOT}/ui/QCommon)
endfunction()
