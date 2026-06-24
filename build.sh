#!/bin/bash

# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

# Author: Biswajit Roy (biswroy@qti.qualcomm.com)

set -e

if [ -z "$QTBIN" ]; then
    echo ""
    echo "ERROR: QTBIN is not set."
    echo "       QTBIN must point to the Qt bin directory, e.g.:"
    echo "         export QTBIN=/path/to/Qt/<version>/gcc_64/bin"
    echo "       Then re-run this script."
    exit 1
fi

if [ ! -d "$QTBIN" ]; then
    echo ""
    echo "ERROR: QTBIN directory does not exist: $QTBIN"
    echo "       Install Qt 6.9+ via the Qt Online Installer (https://www.qt.io/download-qt-installer-oss)"
    echo "       and include the GCC 64-bit component, then update QTBIN."
    exit 1
fi

if ! echo "$QTBIN" | grep -q "gcc_64"; then
    echo ""
    echo "ERROR: QTBIN does not point to a GCC 64-bit Qt installation."
    echo "       QTBIN is currently: $QTBIN"
    echo "       A Linux build requires the Qt GCC 64-bit component. QTBIN must contain 'gcc_64', e.g.:"
    echo "         export QTBIN=/path/to/Qt/<version>/gcc_64/bin"
    exit 1
fi

if ! command -v ninja &>/dev/null; then
    echo ""
    echo "ERROR: ninja not found in PATH."
    echo "       Install ninja via your package manager, e.g.:"
    echo "         sudo apt install ninja-build"
    echo "       Or via the Qt installer (Tools > Ninja)."
    exit 1
fi

export PATH="$QTBIN:$PATH"

# Clean start
rm -rf build __Builds

# Debug
cmake -S . -B build/Debug \
    -DCMAKE_PREFIX_PATH="$(dirname "$QTBIN")" \
    -DCMAKE_COLOR_DIAGNOSTICS=ON \
    -DCMAKE_GENERATOR=Ninja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_FLAGS_INIT=-DQT_QML_DEBUG
cmake --build build/Debug

# Release
cmake -S . -B build/Release \
    -DCMAKE_PREFIX_PATH="$(dirname "$QTBIN")" \
    -DCMAKE_COLOR_DIAGNOSTICS=ON \
    -DCMAKE_GENERATOR=Ninja \
    -DCMAKE_BUILD_TYPE=Release
cmake --build build/Release

echo "Check __Builds directory"
