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


# Common function to set up QTAC applications
function(qtac_add_application)
    set(options WIN32_EXECUTABLE)
    set(oneValueArgs TARGET OUTPUT_NAME VERSION)
    set(multiValueArgs SOURCES HEADERS UI_FILES RESOURCES QT_COMPONENTS LIBRARIES SYSTEM_LIBRARIES)
    
    cmake_parse_arguments(APP "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    
    # Create executable
    if(APP_WIN32_EXECUTABLE AND WIN32)
        add_executable(${APP_TARGET} WIN32 ${APP_SOURCES})
    else()
        add_executable(${APP_TARGET} ${APP_SOURCES})
    endif()
    
    # Add headers
    if(APP_HEADERS)
        target_sources(${APP_TARGET} PRIVATE ${APP_HEADERS})
    endif()
    
    # Process UI files if any
    if(APP_UI_FILES)
        qt6_add_resources(${APP_TARGET} "${APP_TARGET}-ui"
            PREFIX "/ui"
            FILES ${APP_UI_FILES}
        )
    endif()
    
    # Add resources if any
    if(APP_RESOURCES)
        qt6_add_resources(${APP_TARGET} "${APP_TARGET}-resources"
            PREFIX "/"
            FILES ${APP_RESOURCES}
        )
    endif()
    
    # Set common target properties
    set_target_properties(${APP_TARGET} PROPERTIES
        OUTPUT_NAME ${APP_OUTPUT_NAME}
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        AUTOMOC ON
        AUTORCC ON
        AUTOUIC ON
    )
    
    # Set version if provided
    if(APP_VERSION)
        set_target_properties(${APP_TARGET} PROPERTIES VERSION ${APP_VERSION})
    endif()
    
    # Link Qt libraries
    if(APP_QT_COMPONENTS)
        foreach(component ${APP_QT_COMPONENTS})
            target_link_libraries(${APP_TARGET} PRIVATE Qt6::${component})
        endforeach()
    endif()
    
    # Link project libraries
    if(APP_LIBRARIES)
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            foreach(lib ${APP_LIBRARIES})
                if(lib STREQUAL "UI")
                    target_link_libraries(${APP_TARGET} PRIVATE UId)
                elseif(lib STREQUAL "QCommon")
                    target_link_libraries(${APP_TARGET} PRIVATE QCommond)
                elseif(lib STREQUAL "QCommon-console")
                    target_link_libraries(${APP_TARGET} PRIVATE QCommon-consoled)
                else()
                    target_link_libraries(${APP_TARGET} PRIVATE ${lib})
                endif()
            endforeach()
        else()
            target_link_libraries(${APP_TARGET} PRIVATE ${APP_LIBRARIES})
        endif()
    endif()
    
    # Link FTDI library if found
    if(FTDI_FOUND)
        target_link_libraries(${APP_TARGET} PRIVATE FTDI::ftd2xx)
    endif()
    
    # Include current directory
    target_include_directories(${APP_TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    
    # Platform-specific configurations
    if(WIN32)
        if(APP_WIN32_EXECUTABLE)
            set_target_properties(${APP_TARGET} PROPERTIES WIN32_EXECUTABLE TRUE)
        endif()
        
        # Common Windows system libraries
        target_link_libraries(${APP_TARGET} PRIVATE SetupAPI Winusb Ole32)
        
        # Additional system libraries if specified
        if(APP_SYSTEM_LIBRARIES)
            target_link_libraries(${APP_TARGET} PRIVATE ${APP_SYSTEM_LIBRARIES})
        endif()
        
    elseif(UNIX AND NOT APPLE)
        # Common Linux libraries
        target_link_libraries(${APP_TARGET} PRIVATE dl)
        
        # Additional system libraries if specified
        if(APP_SYSTEM_LIBRARIES)
            target_link_libraries(${APP_TARGET} PRIVATE ${APP_SYSTEM_LIBRARIES})
        endif()
        
        # Set RPATH for runtime library loading
        set_target_properties(${APP_TARGET} PROPERTIES
            INSTALL_RPATH "$ORIGIN:$ORIGIN/lib:$ORIGIN/../lib"
            BUILD_WITH_INSTALL_RPATH TRUE
        )
    endif()
    
    # Install target
    install(TARGETS ${APP_TARGET}
        RUNTIME DESTINATION bin
        BUNDLE DESTINATION .
    )
endfunction()
