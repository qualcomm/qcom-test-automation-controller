# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

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
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "ARM64|aarch64")
    message(STATUS "Building ARM64")
    set(WINTARGET "ARM64")
else()
    message(STATUS "Building x86_64")
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

if(WIN32)
    if(TARGET Qt6::windeployqt)
        get_target_property(WINDEPLOYQT_EXECUTABLE Qt6::windeployqt IMPORTED_LOCATION)
    else()
        find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${Qt6_DIR}/../../../bin")
    endif()
    
    if(NOT WINDEPLOYQT_EXECUTABLE)
        set(WINDEPLOYQT_EXECUTABLE windeployqt)
    endif()
endif()

# Function to apply common settings to a target
function(QCommonSettings target_name)
    target_compile_definitions(${target_name} PRIVATE ${QCOMMON_DEFINITIONS})
    
    if(UNIX AND QCOMMON_CXX_FLAGS)
        target_compile_options(${target_name} PRIVATE ${QCOMMON_CXX_FLAGS})
    endif()
    
    target_include_directories(${target_name} PRIVATE ${BUILDROOT}/ui/QCommon)
endfunction()
