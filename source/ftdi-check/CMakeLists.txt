# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

add_executable(FTDICheck
    FTDICheckApplication.cpp
    FTDICheckApplication.h
    main.cpp
)

set_target_properties(FTDICheck PROPERTIES
    AUTOMOC OFF
    AUTOUIC OFF
    AUTORCC OFF
)

target_include_directories(FTDICheck PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../library/include
)

if(WIN32)
    target_link_libraries(FTDICheck PRIVATE
        Psapi     # EnumDeviceDrivers, GetDeviceDriverBaseName
        Version   # GetFileVersionInfo*, VerQueryValue*
    )
endif()

install(TARGETS FTDICheck
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
