#!/bin/bash

# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

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

###############################################################################
# Clean start
###############################################################################

rm -rf build __Builds

###############################################################################
# Debug Build
###############################################################################

cmake -S . -B build/Debug \
    -DCMAKE_PREFIX_PATH="$(dirname "$QTBIN")" \
    -DCMAKE_COLOR_DIAGNOSTICS=ON \
    -DCMAKE_GENERATOR=Ninja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_FLAGS_INIT=-DQT_QML_DEBUG
cmake --build build/Debug

###############################################################################
# Release Build
###############################################################################

cmake -S . -B build/Release \
    -DCMAKE_PREFIX_PATH="$(dirname "$QTBIN")" \
    -DCMAKE_COLOR_DIAGNOSTICS=ON \
    -DCMAKE_GENERATOR=Ninja \
    -DCMAKE_BUILD_TYPE=Release
cmake --build build/Release

###############################################################################
# Qt Runtime Deployment
###############################################################################

echo ""
echo "=========================================================="
echo "Deploying Qt Runtime"
echo "=========================================================="

QT_ROOT="$(dirname "$QTBIN")"

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
    echo "Analyzing plugin: $plugin"

    while read -r dep
    do
        [ -e "$dep" ] && copy_qt_dependency "$dep"
    done < <(
        ldd "$plugin" 2>/dev/null |
        awk '/=>/ {print $3}'
    )
done

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
