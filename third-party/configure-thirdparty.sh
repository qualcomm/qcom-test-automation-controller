#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

# Define directories
ROOT_DIR="$(pwd)"
THIRD_PARTY_DIR="$ROOT_DIR/third-party"
ZIP_FILE="$THIRD_PARTY_DIR/libftd2xx-linux-x86_64-1.4.33.tgz"
TEMP_DIR="$THIRD_PARTY_DIR/linux-x86_64"
DEBUG_LIB_DIR="$ROOT_DIR/__Builds/Linux/Debug/lib"
RELEASE_LIB_DIR="$ROOT_DIR/__Builds/Linux/Release/lib"
FTDI_URL="https://ftdichip.com/wp-content/uploads/2025/03/libftd2xx-linux-x86_64-1.4.33.tgz"

# Check if LIBs already exist
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
cd $THIRD_PARTY_DIR && tar -xvzf "$ZIP_FILE"

# Copy LIBs to lib directories
echo "Copying LIBs to lib directories..."
LIB_PATH=$(find "$TEMP_DIR" -iname "libftd2xx-static.a" | head -n 1)
if [[ -f "$LIB_PATH" ]]; then
    [[ ! -f "$DEBUG_LIB_DIR/libftd2xx.a" ]] && cp "$LIB_PATH" "$DEBUG_LIB_DIR/libftd2xx.a"
    [[ ! -f "$RELEASE_LIB_DIR/libftd2xx.a" ]] && cp "$LIB_PATH" "$RELEASE_LIB_DIR/libftd2xx.a"
else
    echo "Warning: libftd2xx-static.a not found in extracted files."
fi

# Clean up
sleep 2
rm -rf "$TEMP_DIR"

echo "FTDI D2XX driver setup complete."
