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

find_package(Qt6 REQUIRED COMPONENTS Core)

add_executable(ftdi-check
    main.cpp
    FTDICheckApplication.cpp
    FTDICheckCommandLine.cpp
)

target_sources(ftdi-check PRIVATE
    FTDICheckApplication.h
    FTDICheckCommandLine.h
)

set_target_properties(ftdi-check PROPERTIES
    OUTPUT_NAME ftdi-check
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

set_target_properties(ftdi-check PROPERTIES
    VERSION 3.0.0
)

target_link_libraries(ftdi-check PRIVATE
    Qt6::Core
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(ftdi-check PRIVATE
        QCommon-consoled
    )
else()
    target_link_libraries(ftdi-check PRIVATE
        QCommon-console
    )
endif()

target_include_directories(ftdi-check PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../libraries/qcommon-console
)

if(WIN32)
    target_link_libraries(ftdi-check PRIVATE
        SetupAPI
        Winusb
        Ole32
        Advapi32
        Shell32
        Wtsapi32
        Version
        Psapi
    )
elseif(UNIX AND NOT APPLE)
    target_link_libraries(ftdi-check PRIVATE
        dl
    )
endif()

install(TARGETS ftdi-check
    RUNTIME DESTINATION bin
)
