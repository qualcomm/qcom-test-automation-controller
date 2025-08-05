#!/bin/bash

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

# Exit immediately if a command exits with a non-zero status
set -e

# Get the name of the current directory
CURRENT_DIR_NAME=$(basename "$(pwd)")

# Check if the current directory is 'third-party' and navigate up if so
if [[ "$CURRENT_DIR_NAME" == "third-party" ]]; then
    echo "Current directory is \"third-party\". Navigating up to the project root."
    # Store the original directory to return to later
    ORIGINAL_DIR="$(pwd)"
    cd ..
fi

# Define directories
ROOT_DIR="$(pwd)"
THIRD_PARTY_DIR="$ROOT_DIR/third-party"
ZIP_FILE="$THIRD_PARTY_DIR/libftd2xx-linux-x86_64-1.4.33.tgz"
TEMP_DIR="$THIRD_PARTY_DIR/linux-x86_64"
DEBUG_LIB_DIR="$ROOT_DIR/__Builds/Linux/Debug/lib"
RELEASE_LIB_DIR="$ROOT_DIR/__Builds/Linux/Release/lib"
FTDI_URL="https://ftdichip.com/wp-content/uploads/2025/03/libftd2xx-linux-x86_64-1.4.33.tgz"

# Check if static libraries already exist
if [[ -f "$DEBUG_LIB_DIR/libftd2xx.a" && -f "$RELEASE_LIB_DIR/libftd2xx.a" ]]; then
    echo "libftd2xx.a already exist in all target directories. Skipping copy."
    exit 0
fi

# Create necessary directories
mkdir -p "$DEBUG_LIB_DIR" "$RELEASE_LIB_DIR" "$TEMP_DIR"

# Download the ZIP file if not already present
if [[ ! -f "$ZIP_FILE" ]]; then
    echo "Downloading FTDI D2XX driver..."
    curl -L -o "$ZIP_FILE" "$FTDI_URL"
fi

# Extract ZIP
echo "Extracting FTDI driver..."
# Change to the third-party directory for extraction, then return
# The `tar` command will create the 'linux-x86_64' directory inside $THIRD_PARTY_DIR
(cd "$THIRD_PARTY_DIR" && tar -xvzf "$ZIP_FILE")

# Copy LIBs to lib directories
echo "Copying static libraries to lib directories..."
LIB_PATH=$(find "$TEMP_DIR" -iname "libftd2xx-static.a" | head -n 1)
if [[ -f "$LIB_PATH" ]]; then
    [[ ! -f "$DEBUG_LIB_DIR/libftd2xx.a" ]] && cp "$LIB_PATH" "$DEBUG_LIB_DIR/libftd2xx.a"
    [[ ! -f "$RELEASE_LIB_DIR/libftd2xx.a" ]] && cp "$LIB_PATH" "$RELEASE_LIB_DIR/libftd2xx.a"
else
    echo "Warning: libftd2xx-static.a not found in extracted files."
    exit 1
fi

# Clean up
sleep 2
rm -rf "$TEMP_DIR"

echo "FTDI D2XX library setup complete."

# Return to the original directory if we changed it
if [[ -n "$ORIGINAL_DIR" ]]; then
    cd "$ORIGINAL_DIR"
fi
