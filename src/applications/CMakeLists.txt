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

cmake_minimum_required(VERSION 3.16)
project(applications VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core)

qt_standard_project_setup()

option(DEPLOY_APPS "Run macdeployqt/windeployqt to create self-contained app bundles (slow)" OFF)

if(APPLE)
    # Add the Qt frameworks directory to the install rpath so apps can find Qt
    # frameworks at runtime without a full macdeployqt deployment.
    get_filename_component(_qt_lib_dir "${Qt6Core_DIR}/../../" ABSOLUTE)
    list(APPEND CMAKE_INSTALL_RPATH "${_qt_lib_dir}")
endif()

# Qt 6.3/6.4 use FILENAME_VARIABLE; Qt 6.5+ renamed it to OUTPUT_SCRIPT.
# On Linux, system installs use system-provided Qt shared libraries;
# running the deploy tool would copy system libs into /usr/local which is wrong.
# On Windows/macOS the deploy step is opt-in via -DDEPLOY_APPS=ON (or --deploy
# in build.sh/build.bat) to avoid slow macdeployqt/windeployqt runs during
# routine testing.
macro(qtac_deploy_app target)
    if(DEPLOY_APPS AND (WIN32 OR APPLE))
        if(Qt6Core_VERSION VERSION_GREATER_EQUAL "6.5")
            qt_generate_deploy_app_script(
                TARGET ${target}
                OUTPUT_SCRIPT deploy_script
                NO_UNSUPPORTED_PLATFORM_ERROR
            )
        else()
            qt_generate_deploy_app_script(
                TARGET ${target}
                FILENAME_VARIABLE deploy_script
                NO_UNSUPPORTED_PLATFORM_ERROR
            )
        endif()
        install(SCRIPT ${deploy_script})
    endif()
endmacro()

if(UNIX AND NOT APPLE)
    # Install a qt.conf pointing to the Qt plugins directory used at build time.
    # Without this, installed binaries report an empty plugin path and fail to
    # start. Works for both apt Qt (/usr/lib/.../qt6/plugins) and aqtinstall.
    get_target_property(_qmake Qt6::qmake IMPORTED_LOCATION)
    execute_process(
        COMMAND "${_qmake}" -query QT_INSTALL_PLUGINS
        OUTPUT_VARIABLE _qt_plugins_dir
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/qt.conf"
        "[Paths]\nPlugins=${_qt_plugins_dir}\n"
    )
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/qt.conf"
        DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()

add_subdirectory(devlist)
add_subdirectory(tacdump)
add_subdirectory(ftdi-check)
add_subdirectory(updatedevicelist)
add_subdirectory(device-catalog)
add_subdirectory(test-automation-controller)
add_subdirectory(tac-configuration-editor)
add_subdirectory(programmers)
