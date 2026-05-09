#!/bin/bash

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

set -e

PRISTINE=1
BUILD_UI=ON
BUILD_DEBUG=0
INSTALL=0
DEPLOY=0

for arg in "$@"; do
    case "$arg" in
        --pristine)    PRISTINE=1 ;;
        --incremental) PRISTINE=0 ;;
        --no-gui)      BUILD_UI=OFF ;;
        --debug)       BUILD_DEBUG=1 ;;
        --install)     INSTALL=1 ;;
        --deploy)      DEPLOY=1 ;;
        *)
            echo "Usage: $0 [--pristine|--incremental] [--no-gui] [--debug] [--install] [--deploy]"
            echo "  --pristine     Delete build/, __Builds/, and cached downloads (default)"
            echo "  --incremental  Reuse existing build tree and downloaded libraries"
            echo "  --no-gui       Build just low-level libraries without the UI application"
            echo "  --debug        Also build Debug configuration (Release is always built)"
            echo "  --install      Install libraries, headers, applications, and configs"
            echo "                 (installs to CMAKE_INSTALL_PREFIX, default: /usr/local)"
            echo "  --deploy       Bundle Qt dependencies into each app (slow; for distribution)"
            exit 1 ;;
    esac
done

if [ -z "$QTBIN" ]; then
    echo "Set QTBIN first"
    exit 1
fi

export PATH="$QTBIN:$PATH"

case "$(uname)" in
    Linux)
        DISTRO=$(. /etc/os-release && echo "$ID")
        CMAKE_DISTRO_FLAG="-DLINUX_DISTRO=${DISTRO}"
        NPROC=$(nproc)
        BUILDS_SUBDIR="Linux-${DISTRO}"
        ;;
    Darwin)
        DISTRO="macOS"
        CMAKE_DISTRO_FLAG=""
        NPROC=$(sysctl -n hw.logicalcpu)
        BUILDS_SUBDIR="macOS"
        ;;
    *)
        echo "Unsupported platform: $(uname)"
        exit 1
        ;;
esac

if [ "$PRISTINE" -eq 1 ]; then
    rm -rf build/${DISTRO} __Builds/${BUILDS_SUBDIR}
    rm -f third-party/*.tgz third-party/*.zip
fi

# Debug
if [ "$BUILD_DEBUG" -eq 1 ]; then
    cmake -S . -B build/${DISTRO}/Debug -DCMAKE_PREFIX_PATH="$(dirname "$QTBIN")" -DCMAKE_BUILD_TYPE=Debug ${CMAKE_DISTRO_FLAG} -DBUILD_UI=${BUILD_UI} -DDEPLOY_APPS=OFF
    cmake --build build/${DISTRO}/Debug --parallel ${NPROC}
fi

# Release
if [ "$DEPLOY" -eq 1 ]; then
    DEPLOY_APPS_FLAG=ON
else
    DEPLOY_APPS_FLAG=OFF
fi
cmake -S . -B build/${DISTRO}/Release -DCMAKE_PREFIX_PATH="$(dirname "$QTBIN")" -DCMAKE_BUILD_TYPE=Release ${CMAKE_DISTRO_FLAG} -DBUILD_UI=${BUILD_UI} -DDEPLOY_APPS=${DEPLOY_APPS_FLAG}
cmake --build build/${DISTRO}/Release --parallel ${NPROC}

if [ "$INSTALL" -eq 1 ]; then
    sudo cmake --install build/${DISTRO}/Release
fi

echo "Check __Builds directory"
