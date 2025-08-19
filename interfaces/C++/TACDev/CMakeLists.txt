# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries. 
#  
# Redistribution and use in source and binary forms, with or without
# modification, are permitted (subject to the limitations in the
# disclaimer below) provided that the following conditions are met:
#  
# 	* Redistributions of source code must retain the above copyright
# 	  notice, this list of conditions and the following disclaimer.
#  
# 	* Redistributions in binary form must reproduce the above
# 	  copyright notice, this list of conditions and the following
# 	  disclaimer in the documentation and/or other materials provided
# 	  with the distribution.
#  
# 	* Neither the name of Qualcomm Technologies, Inc. nor the names of its
# 	  contributors may be used to endorse or promote products derived
# 	  from this software without specific prior written permission.
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

# Author: Biswajit Roy <biswroy@qti.qualcomm.com>


# Find Qt components
find_package(Qt6 REQUIRED COMPONENTS Core SerialPort)

# Define the library target
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(TARGET_NAME TACDevd)
else()
    set(TARGET_NAME TACDev)
endif()

add_library(${TARGET_NAME} SHARED
    TACDev.cpp
    TACDevCore.cpp
)

# Add headers
target_sources(${TARGET_NAME} PRIVATE
    TACDev.h
    TACDevCore.h
)

# Set target properties
set_target_properties(${TARGET_NAME} PROPERTIES
    OUTPUT_NAME ${TARGET_NAME}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

# Add compile definitions
target_compile_definitions(${TARGET_NAME} PRIVATE
    TACDEV_LIBRARY
)

# Link Qt libraries
target_link_libraries(${TARGET_NAME} PUBLIC
    Qt6::Core
    Qt6::SerialPort
    Qt6::Network
)

# Link to project libraries
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(${TARGET_NAME} PUBLIC
        QCommon-consoled
    )
else()
    target_link_libraries(${TARGET_NAME} PUBLIC
        QCommon-console
    )
endif()

# Link FTDI library if found
if(FTDI_FOUND)
    target_link_libraries(${TARGET_NAME} PUBLIC FTDI::ftd2xx)
endif()

# Include directories
target_include_directories(${TARGET_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/libraries/qcommon-console
)

# Platform-specific configurations
if(WIN32)
    # Add Windows-specific system libraries
    target_link_libraries(${TARGET_NAME} PUBLIC
        SetupAPI
        Winusb
        Ole32
        Advapi32
        Shell32
        Wtsapi32
    )
elseif(UNIX AND NOT APPLE)
    # Linux-specific libraries
    target_link_libraries(${TARGET_NAME} PUBLIC
        rt
        dl
        uuid
    )
elseif(APPLE)
    # macOS-specific configurations
    target_link_libraries(${TARGET_NAME} PUBLIC
        "-framework CoreFoundation"
        "-framework IOKit"
        objc
    )
endif()

# Export target for use by other parts of the project
set_target_properties(${TARGET_NAME} PROPERTIES EXPORT_NAME TACDev)

# Install target
install(TARGETS ${TARGET_NAME}
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
)

# Install headers
install(FILES
    TACDev.h
    TACDevCore.h
    DESTINATION include/TACDev
)
