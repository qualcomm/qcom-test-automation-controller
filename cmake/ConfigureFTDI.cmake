# ConfigureFTDI.cmake - Find and configure FTDI D2XX library
# This module defines:
#  FTDI_FOUND - True if FTDI library is found
#  FTDI::ftd2xx - Imported target for FTDI library

# Download and extract FTDI drivers if needed
if(WIN32)
    set(FTDI_EXTRACT_DIR "${CMAKE_SOURCE_DIR}/third-party/ftdi")
    set(FTDI_ZIP_FILE "${CMAKE_SOURCE_DIR}/third-party/CDM-v2.12.36.20-WHQL-Certified.zip")
    set(FTDI_DOWNLOAD_URL "https://ftdichip.com/wp-content/uploads/2025/03/CDM-v2.12.36.20-WHQL-Certified.zip")
    
    # Download FTDI drivers if not present
    if(NOT EXISTS "${FTDI_ZIP_FILE}")
        message(STATUS "Downloading FTDI drivers from ${FTDI_DOWNLOAD_URL}...")
        file(DOWNLOAD 
            "${FTDI_DOWNLOAD_URL}" 
            "${FTDI_ZIP_FILE}"
            SHOW_PROGRESS
            STATUS DOWNLOAD_STATUS
        )
        
        list(GET DOWNLOAD_STATUS 0 DOWNLOAD_ERROR)
        if(NOT DOWNLOAD_ERROR EQUAL 0)
            list(GET DOWNLOAD_STATUS 1 DOWNLOAD_ERROR_MSG)
            message(WARNING "Failed to download FTDI drivers: ${DOWNLOAD_ERROR_MSG}")
            message(STATUS "Please manually download from: ${FTDI_DOWNLOAD_URL}")
            message(STATUS "And place it at: ${FTDI_ZIP_FILE}")
        else()
            message(STATUS "FTDI drivers downloaded successfully")
        endif()
    endif()
    
    # Extract FTDI drivers if zip file exists
    if(EXISTS "${FTDI_ZIP_FILE}" AND NOT EXISTS "${FTDI_EXTRACT_DIR}")
        message(STATUS "Extracting FTDI drivers...")
        file(ARCHIVE_EXTRACT 
            INPUT "${FTDI_ZIP_FILE}"
            DESTINATION "${FTDI_EXTRACT_DIR}"
        )
        message(STATUS "FTDI drivers extracted to: ${FTDI_EXTRACT_DIR}")
    endif()
endif()

# Look for FTDI library in common locations
if(WIN32)
    # Windows - look for static library
    find_library(FTDI_LIBRARY
        NAMES ftd2xx FTD2XX
        PATHS
            "${CMAKE_SOURCE_DIR}/third-party/ftdi"
            "${CMAKE_SOURCE_DIR}/third-party/ftdi/amd64"
            "${CMAKE_SOURCE_DIR}/third-party/ftdi/i386"
            "${CMAKE_SOURCE_DIR}/third-party/FTDI"
        PATH_SUFFIXES
            lib
            amd64
            i386
            x64
            x86
            Static/amd64
            Static/i386
        NO_DEFAULT_PATH
    )
    
    # Look for header
    find_path(FTDI_INCLUDE_DIR
        NAMES ftd2xx.h
        PATHS
            "${CMAKE_SOURCE_DIR}/src/libraries/qcommon-console"
            "${CMAKE_SOURCE_DIR}/third-party/ftdi"
            "${CMAKE_SOURCE_DIR}/third-party/FTDI"
            "C:/Program Files/FTDI/CDM"
            "C:/FTDI"
        PATH_SUFFIXES
            include
        NO_DEFAULT_PATH
    )
    
elseif(UNIX AND NOT APPLE)
    # Linux - look for shared library
    find_library(FTDI_LIBRARY
        NAMES ftd2xx libftd2xx.so libftd2xx.so.1.4.27
        PATHS
            /usr/local/lib
            /usr/lib
            "${CMAKE_SOURCE_DIR}/third-party/ftdi"
        NO_DEFAULT_PATH
    )
    
    find_path(FTDI_INCLUDE_DIR
        NAMES ftd2xx.h
        PATHS
            "${CMAKE_SOURCE_DIR}/src/libraries/qcommon-console"
            /usr/local/include
            /usr/include
            "${CMAKE_SOURCE_DIR}/third-party/ftdi"
        NO_DEFAULT_PATH
    )
endif()

# Handle the QUIETLY and REQUIRED arguments
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FTDI
    FOUND_VAR FTDI_FOUND
    REQUIRED_VARS FTDI_LIBRARY FTDI_INCLUDE_DIR
)

# Create imported target and configure FTDI
if(FTDI_FOUND AND NOT TARGET FTDI::ftd2xx)
    add_library(FTDI::ftd2xx UNKNOWN IMPORTED)
    set_target_properties(FTDI::ftd2xx PROPERTIES
        IMPORTED_LOCATION "${FTDI_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${FTDI_INCLUDE_DIR}"
    )
    
    # Add platform-specific link libraries
    if(WIN32)
        set_target_properties(FTDI::ftd2xx PROPERTIES
            INTERFACE_LINK_LIBRARIES "SetupAPI;Winusb;Ole32"
        )
    endif()
    
    # Copy FTDI library files to build directories
    if(WIN32 AND FTDI_LIBRARY)
        get_filename_component(FTDI_LIB_DIR "${FTDI_LIBRARY}" DIRECTORY)
        
        # Look for specific FTDI DLL files to copy (only ftd2xx.dll)
        set(FTDI_DLLS)
        
        # Find ftd2xx.dll
        find_file(FTDI_DLL_SIMPLE
            NAMES ftd2xx.dll
            PATHS
                "${CMAKE_SOURCE_DIR}/third-party/ftdi/i386"
                "${CMAKE_SOURCE_DIR}/third-party/ftdi/amd64"
                "${FTDI_LIB_DIR}"
                "${FTDI_LIB_DIR}/../"
            NO_DEFAULT_PATH
        )
        
        if(FTDI_DLL_SIMPLE)
            list(APPEND FTDI_DLLS ${FTDI_DLL_SIMPLE})
        endif()
        
        # Look for .lib files to copy
        file(GLOB FTDI_LIBS 
            "${FTDI_LIB_DIR}/*.lib"
            "${CMAKE_SOURCE_DIR}/third-party/ftdi/amd64/*.lib"
            "${CMAKE_SOURCE_DIR}/third-party/ftdi/Static/amd64/*.lib"
        )
        
        # Copy FTDI DLLs to bin directories (only ftd2xx.dll)
        if(FTDI_DLLS)
            foreach(dll ${FTDI_DLLS})
                # Always copy as lowercase ftd2xx.dll
                configure_file("${dll}" "${CMAKE_SOURCE_DIR}/__Builds/x64/Windows/Debug/bin/ftd2xx.dll" COPYONLY)
                configure_file("${dll}" "${CMAKE_SOURCE_DIR}/__Builds/x64/Windows/Release/bin/ftd2xx.dll" COPYONLY)
            endforeach()
            message(STATUS "FTDI DLL copied to bin directories as ftd2xx.dll: ${FTDI_DLLS}")
        endif()
        
        # Copy FTDI .lib files to lib directories
        if(FTDI_LIBS)
            foreach(lib ${FTDI_LIBS})
                get_filename_component(lib_name "${lib}" NAME)
                configure_file("${lib}" "${CMAKE_SOURCE_DIR}/__Builds/x64/Windows/Debug/lib/${lib_name}" COPYONLY)
                configure_file("${lib}" "${CMAKE_SOURCE_DIR}/__Builds/x64/Windows/Release/lib/${lib_name}" COPYONLY)
            endforeach()
            message(STATUS "FTDI .lib files copied to lib directories: ${FTDI_LIBS}")
        endif()
        
        message(STATUS "FTDI library found: ${FTDI_LIBRARY}")
    endif()
    
    message(STATUS "FTDI D2XX library found and configured")
else()
    message(WARNING "FTDI D2XX library not found. Some applications may not build correctly.")
    message(STATUS "To install FTDI libraries:")
    message(STATUS "  Windows: Download from https://ftdichip.com/drivers/d2xx-drivers/")
    message(STATUS "  Linux: Install libftd2xx or use the setup scripts in third-party/")
endif()

mark_as_advanced(FTDI_LIBRARY FTDI_INCLUDE_DIR)
