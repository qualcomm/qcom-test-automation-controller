#!/bin/bash

# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

# Author: Biswajit Roy (biswroy@qti.qualcomm.com)

set -e

if [ -z "$QTBIN" ]; then
    echo "Set QTBIN first"
    exit 1
fi

export PATH="$QTBIN:$PATH"

# Clean start
rm -rf build __Builds

# Debug
cmake -S . -B build/Debug -DCMAKE_PREFIX_PATH="$(dirname "$QTBIN")" -DCMAKE_BUILD_TYPE=Debug
cmake --build build/Debug

# Release
cmake -S . -B build/Release -DCMAKE_PREFIX_PATH="$(dirname "$QTBIN")" -DCMAKE_BUILD_TYPE=Release
cmake --build build/Release

echo "Check __Builds directory"
