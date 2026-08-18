#!/bin/bash

# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

set -e

###############################################################################
# Locate Qt: use QTBIN (Qt Online Installer tree) if set, else host Qt
###############################################################################

USING_SYSTEM_QT=0
MISSING_PACKAGES=()

command -v cmake &>/dev/null || MISSING_PACKAGES+=("cmake")
command -v ninja &>/dev/null || MISSING_PACKAGES+=("ninja-build")
command -v g++   &>/dev/null || MISSING_PACKAGES+=("build-essential")

if [ -n "$QTBIN" ]; then
    if [ ! -d "$QTBIN" ]; then
        echo ""
        echo "ERROR: QTBIN directory does not exist: $QTBIN"
        echo "       Install Qt 6.4+ via the Qt Online Installer (https://www.qt.io/download-qt-installer-oss)"
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

    QT_PREFIX="$(dirname "$QTBIN")"
    export PATH="$QTBIN:$PATH"
else
    USING_SYSTEM_QT=1

    if command -v qmake6 &>/dev/null; then
        QT_PREFIX="$(qmake6 -query QT_INSTALL_PREFIX)"
    else
        MISSING_PACKAGES+=("qt6-base-dev")
    fi

    dpkg -s qt6-multimedia-dev &>/dev/null || MISSING_PACKAGES+=("qt6-multimedia-dev")
    dpkg -s qt6-serialport-dev &>/dev/null || MISSING_PACKAGES+=("qt6-serialport-dev")
fi

if [ "${#MISSING_PACKAGES[@]}" -gt 0 ]; then
    echo ""
    echo "ERROR: Missing required build tools/packages: ${MISSING_PACKAGES[*]}"
    echo "       Install them with:"
    echo "         sudo apt install ${MISSING_PACKAGES[*]}"
    echo "       Then re-run this script."
    echo ""
    echo "       (Alternatively, install Qt via the Qt Online Installer"
    echo "       (https://www.qt.io/download-qt-installer-oss) and set QTBIN to its"
    echo "       gcc_64/bin directory instead of using the system Qt packages.)"
    exit 1
fi

if [ "$USING_SYSTEM_QT" -eq 1 ]; then
    echo "Using host Qt installation: $QT_PREFIX"
fi

###############################################################################
# Clean start
###############################################################################

rm -rf build __Builds

###############################################################################
# Debug Build
###############################################################################

cmake -S . -B build/Debug \
    -DCMAKE_PREFIX_PATH="$QT_PREFIX" \
    -DCMAKE_COLOR_DIAGNOSTICS=ON \
    -DCMAKE_GENERATOR=Ninja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_FLAGS_INIT=-DQT_QML_DEBUG
cmake --build build/Debug

###############################################################################
# Release Build
###############################################################################

cmake -S . -B build/Release \
    -DCMAKE_PREFIX_PATH="$QT_PREFIX" \
    -DCMAKE_COLOR_DIAGNOSTICS=ON \
    -DCMAKE_GENERATOR=Ninja \
    -DCMAKE_BUILD_TYPE=Release
cmake --build build/Release

if [ "$USING_SYSTEM_QT" -eq 1 ]; then
    echo ""
    echo "=========================================================="
    echo "Using system Qt — skipping runtime bundling"
    echo "=========================================================="
    echo "Binaries link against the host's installed Qt6 packages."
    echo "Check __Builds directory"
    exit 0
fi

###############################################################################
# Qt Runtime Deployment
###############################################################################

echo ""
echo "=========================================================="
echo "Deploying Qt Runtime"
echo "=========================================================="

QT_ROOT="$QT_PREFIX"

DEPLOY_BIN_DIR="__Builds/Linux/Release/bin"
DEPLOY_LIB_DIR="__Builds/Linux/Release/lib"
DEPLOY_PLUGIN_DIR="__Builds/Linux/Release/plugins"

mkdir -p "$DEPLOY_LIB_DIR"
mkdir -p "$DEPLOY_PLUGIN_DIR"

declare -A COPIED

copy_qt_dependency()
{
    local dep="$1"

    [ -e "$dep" ] || return

    case "$dep" in
        "$QT_ROOT"/*)
            ;;
        *)
            return
            ;;
    esac

    local real_dep
    real_dep="$(readlink -f "$dep")"

    if [ -n "${COPIED[$real_dep]:-}" ]; then
        return
    fi

    COPIED["$real_dep"]=1

    echo "Copying $(basename "$real_dep")"

    #
    # Copy the real library file
    #
    cp -a "$real_dep" "$DEPLOY_LIB_DIR/" \
        2>/dev/null || true

    #
    # Copy all symlinks belonging to this library
    #
    local base_lib
    base_lib="$(basename "$real_dep")"
    base_lib="${base_lib%%.so*}"

    find "$QT_ROOT/lib" \
        -maxdepth 1 \
        -name "${base_lib}.so*" \
        -exec cp -a {} "$DEPLOY_LIB_DIR/" \; \
        2>/dev/null || true

    #
    # Recurse through dependencies
    #
    while read -r child
    do
        [ -e "$child" ] && copy_qt_dependency "$child"
    done < <(
        ldd "$real_dep" 2>/dev/null |
        awk '/=>/ {print $3}'
    )
}

echo ""
echo "Scanning executables..."

find "$DEPLOY_BIN_DIR" -type f -executable | while read -r exe
do
    echo ""
    echo "Analyzing: $exe"

    while read -r dep
    do
        [ -f "$dep" ] && copy_qt_dependency "$dep"
    done < <(
        ldd "$exe" |
        awk '/=>/ {print $3}'
    )
done

###############################################################################
# Deploy Qt Plugins
###############################################################################

echo ""
echo "Deploying Qt plugins..."

for plugin_dir in \
    iconengines \
    imageformats \
    platforminputcontexts \
    platforms \
    platformthemes \
    xcbglintegrations
do
    if [ -d "$QT_ROOT/plugins/$plugin_dir" ]; then

        mkdir -p "$DEPLOY_PLUGIN_DIR/$plugin_dir"

        echo "Copying plugin directory: $plugin_dir"

        cp -a \
            "$QT_ROOT/plugins/$plugin_dir/." \
            "$DEPLOY_PLUGIN_DIR/$plugin_dir/"
    fi
done

###############################################################################
# Scan Qt Plugin Dependencies
###############################################################################

echo ""
echo "Scanning Qt plugin dependencies..."

find "$DEPLOY_PLUGIN_DIR" -type f -name "*.so*" | while read -r plugin
do
    while read -r dep
    do
        [ -e "$dep" ] && copy_qt_dependency "$dep"

    done < <(
        ldd "$plugin" 2>/dev/null |
        awk '/=>/ {print $3}'
    )
done

###############################################################################
# Ensure Qt XCB Support Libraries Are Present
###############################################################################

echo ""
echo "Checking for Qt XCB support libraries..."

find "$QT_ROOT/lib" \
     -maxdepth 1 \
     -name "libQt6XcbQpa.so*" \
     -exec cp -a {} "$DEPLOY_LIB_DIR/" \;

echo "Qt XCB support libraries copied."

###############################################################################
# Copy Qt Runtime Libraries
###############################################################################

echo ""
echo "Copying Qt runtime libraries..."

find "$QT_ROOT/lib" \
    -maxdepth 1 \
    -name "libQt6*.so*" \
    -exec cp -a {} "$DEPLOY_LIB_DIR/" \;

echo "Qt runtime libraries copied."

###############################################################################
# Deploy ICU Libraries (extra safety)
###############################################################################

echo ""
echo "Checking ICU libraries..."

for icu_lib in \
    libicui18n.so \
    libicuuc.so \
    libicudata.so
do
    find "$QT_ROOT/lib" -name "${icu_lib}*" 2>/dev/null | while read -r f
    do
        cp -a "$f" "$DEPLOY_LIB_DIR/" \
            2>/dev/null || true
    done
done

###############################################################################
# Validate deployment
###############################################################################

echo ""
echo "Checking for broken library symlinks..."

BROKEN_SYMLINKS=$(find "$DEPLOY_LIB_DIR" -xtype l 2>/dev/null || true)

if [ -n "$BROKEN_SYMLINKS" ]; then

    echo ""
    echo "ERROR: Broken library symlinks detected:"
    echo "$BROKEN_SYMLINKS"
    exit 1

fi

echo "No broken library symlinks detected."

###############################################################################
# Summary
###############################################################################

echo ""
echo "=========================================================="
echo "Qt Deployment Complete"
echo "=========================================================="

echo ""
echo "Release Output:"
echo "  __Builds/Linux/Release"

echo ""
echo "Libraries:"
echo "  $DEPLOY_LIB_DIR"

echo ""
echo "Plugins:"
echo "  $DEPLOY_PLUGIN_DIR"

echo ""
echo "Library Count:"
find "$DEPLOY_LIB_DIR" -type f | wc -l

echo ""
echo "Plugin Count:"
find "$DEPLOY_PLUGIN_DIR" -type f | wc -l

echo ""
echo "Check __Builds directory"
