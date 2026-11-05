#!/usr/bin/env bash

# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$SCRIPT_DIR"
CONFIGURATION="${1:-Release}"

usage() {
    cat <<'USAGE'
Usage: ./build-macos.sh [Debug|Release|all]

Build QTAC for the native macOS architecture. QTBIN must point to the Qt
macOS bin directory. The FTDI D2XX library is taken from FTD2XX_LIBRARY or
FTD2XX_ROOT when set, then from a local D2XX1.4.36 extraction, otherwise
downloaded from FTDI and extracted from its DMG.

Environment:
  QTBIN             Qt macOS bin directory (required)
  MACOS_ARCH        Target architecture: arm64 or x86_64 (default: host)
  FTD2XX_LIBRARY    Explicit path to libftd2xx.dylib
  FTD2XX_ROOT       Root directory containing the D2XX dylib
  FTD2XX_URL        Override the FTDI DMG URL
  FTD2XX_CACHE_DIR  Download/extraction cache directory
  BUILD_ROOT        CMake build directory (default: build/macos-<arch>-<config>-ninja)
USAGE
}

case "$CONFIGURATION" in
    -h|--help|help)
        usage
        exit 0
        ;;
    Debug|Release|all)
        ;;
    *)
        echo "ERROR: Unsupported build configuration: $CONFIGURATION" >&2
        usage >&2
        exit 2
        ;;
esac

if [[ -z "${QTBIN:-}" || ! -d "$QTBIN" ]]; then
    echo "ERROR: QTBIN must point to the Qt macOS bin directory." >&2
    echo "Example: export QTBIN=/Users/you/Qt/6.12.0/macos/bin" >&2
    exit 1
fi

QT_ROOT="$(cd "$QTBIN/.." && pwd)"
QT_INSTALL_ROOT="$(cd "$QTBIN/../../.." && pwd)"
for extra_path in "$QTBIN" "$QT_INSTALL_ROOT/Tools/Ninja" /opt/homebrew/bin /usr/local/bin; do
    if [[ -d "$extra_path" ]]; then
        PATH="$extra_path:$PATH"
    fi
done
export PATH

for tool in cmake ninja curl hdiutil lipo otool install_name_tool file; do
    if ! command -v "$tool" >/dev/null 2>&1; then
        echo "ERROR: Required tool '$tool' was not found in PATH." >&2
        exit 1
    fi
done

if [[ ! -d "$QT_ROOT/lib/cmake/Qt6" ]]; then
    echo "ERROR: Qt 6 CMake package was not found under $QT_ROOT/lib/cmake/Qt6." >&2
    echo "Install Qt 6.9+ for macOS, including Qt Multimedia and Qt Serial Port." >&2
    exit 1
fi

MACOS_ARCH="${MACOS_ARCH:-$(uname -m)}"
case "$MACOS_ARCH" in
    arm64)
        BUILD_ARCH="ARM64"
        ;;
    x86_64)
        BUILD_ARCH="x64"
        ;;
    *)
        echo "ERROR: Unsupported macOS architecture '$MACOS_ARCH' (expected arm64 or x86_64)." >&2
        exit 1
        ;;
esac

FTD2XX_URL="${FTD2XX_URL:-https://ftdichip.com/wp-content/uploads/2026/09/D2XX1.4.36.dmg}"
FTD2XX_CACHE_DIR="${FTD2XX_CACHE_DIR:-${HOME:?HOME is not set}/Library/Caches/qtac/ftdi/1.4.36}"
TEMP_DIR=""
MOUNT_POINT=""

cleanup() {
    if [[ -n "$MOUNT_POINT" ]]; then
        hdiutil detach "$MOUNT_POINT" -quiet >/dev/null 2>&1 || true
    fi
    if [[ -n "$TEMP_DIR" && -d "$TEMP_DIR" ]]; then
        rm -rf "$TEMP_DIR"
    fi
}
trap cleanup EXIT

find_d2xx_library() {
    local search_root="$1"
    [[ -d "$search_root" ]] || return 0
    find "$search_root" -type f -name 'libftd2xx*.dylib' -print -quit 2>/dev/null
}

FTD2XX_LIBRARY="${FTD2XX_LIBRARY:-}"
if [[ -n "$FTD2XX_LIBRARY" ]]; then
    if [[ ! -f "$FTD2XX_LIBRARY" ]]; then
        echo "ERROR: FTD2XX_LIBRARY does not exist: $FTD2XX_LIBRARY" >&2
        exit 1
    fi
elif [[ -n "${FTD2XX_ROOT:-}" ]]; then
    FTD2XX_LIBRARY="$(find_d2xx_library "$FTD2XX_ROOT")"
elif [[ -f "$REPO_ROOT/D2XX1.4.36/release/build/libftd2xx.dylib" ]]; then
    FTD2XX_LIBRARY="$REPO_ROOT/D2XX1.4.36/release/build/libftd2xx.dylib"
elif [[ -f "$REPO_ROOT/third-party/D2XX1.4.36/release/build/libftd2xx.dylib" ]]; then
    FTD2XX_LIBRARY="$REPO_ROOT/third-party/D2XX1.4.36/release/build/libftd2xx.dylib"
else
    FTD2XX_LIBRARY="$(find_d2xx_library "$FTD2XX_CACHE_DIR")"
fi

if [[ -z "$FTD2XX_LIBRARY" ]]; then
    mkdir -p "$FTD2XX_CACHE_DIR"
    DMG_PATH="$FTD2XX_CACHE_DIR/D2XX1.4.36.dmg"
    if [[ ! -s "$DMG_PATH" ]]; then
        echo "Downloading FTDI D2XX from $FTD2XX_URL"
        if ! curl --fail --location --retry 3 --output "$DMG_PATH.part" "$FTD2XX_URL"; then
            rm -f "$DMG_PATH.part"
            echo "ERROR: Could not download the FTDI D2XX DMG." >&2
            echo "Download it from the FTDI D2XX driver page, then either set:" >&2
            echo "  FTD2XX_LIBRARY=/path/to/libftd2xx.dylib ./build-macos.sh" >&2
            echo "or extract it and set FTD2XX_ROOT to the extracted directory." >&2
            exit 1
        fi
        mv "$DMG_PATH.part" "$DMG_PATH"
    fi

    TEMP_DIR="$(mktemp -d "${TMPDIR:-/tmp}/qtac-ftdi.XXXXXX")"
    MOUNT_POINT="$TEMP_DIR/mnt"
    mkdir -p "$MOUNT_POINT"
    if ! hdiutil attach "$DMG_PATH" -nobrowse -readonly -mountpoint "$MOUNT_POINT" >/dev/null; then
        echo "ERROR: Unable to mount FTDI disk image: $DMG_PATH" >&2
        exit 1
    fi

    FTD2XX_LIBRARY="$(find_d2xx_library "$MOUNT_POINT")"
    if [[ -z "$FTD2XX_LIBRARY" ]]; then
        FTDI_PACKAGE="$(find "$MOUNT_POINT" -type d -name '*.pkg' -print -quit 2>/dev/null || true)"
        if [[ -n "$FTDI_PACKAGE" ]]; then
            PKG_EXPANDED="$TEMP_DIR/pkg-expanded"
            if pkgutil --expand-full "$FTDI_PACKAGE" "$PKG_EXPANDED" >/dev/null 2>&1; then
                FTD2XX_LIBRARY="$(find_d2xx_library "$PKG_EXPANDED")"
            fi
        fi
    fi

    if [[ -z "$FTD2XX_LIBRARY" ]]; then
        echo "ERROR: No libftd2xx dylib was found in the mounted FTDI package." >&2
        echo "Set FTD2XX_LIBRARY to the dylib path from the vendor package." >&2
        exit 1
    fi

    CACHED_LIBRARY="$FTD2XX_CACHE_DIR/libftd2xx.dylib"
    cp -L "$FTD2XX_LIBRARY" "$CACHED_LIBRARY"
    FTD2XX_LIBRARY="$CACHED_LIBRARY"
fi

if ! lipo "$FTD2XX_LIBRARY" -verify_arch "$MACOS_ARCH" >/dev/null 2>&1; then
    echo "ERROR: $FTD2XX_LIBRARY does not contain the $MACOS_ARCH architecture." >&2
    lipo -info "$FTD2XX_LIBRARY" >&2 || true
    exit 1
fi

if [[ "$CONFIGURATION" == all ]]; then
    CONFIGURATIONS=(Debug Release)
else
    CONFIGURATIONS=("$CONFIGURATION")
fi

for CONFIG in "${CONFIGURATIONS[@]}"; do
    CONFIG_LOWER="$(printf '%s' "$CONFIG" | tr '[:upper:]' '[:lower:]')"
    if [[ -n "${BUILD_ROOT:-}" && "$CONFIGURATION" == all ]]; then
        BUILD_DIR="$BUILD_ROOT/$CONFIG_LOWER"
    else
        BUILD_DIR="${BUILD_ROOT:-$REPO_ROOT/build/macos-${MACOS_ARCH}-${CONFIG_LOWER}-ninja}"
    fi
    OUTPUT_DIR="$REPO_ROOT/__Builds/MacOS/$BUILD_ARCH/$CONFIG"
    OUTPUT_BIN_DIR="$OUTPUT_DIR/bin"
    OUTPUT_LIB_DIR="$OUTPUT_DIR/lib"
    OUTPUT_FTDI="$OUTPUT_LIB_DIR/libftd2xx.dylib"

    echo
    echo "=========================================================="
    echo "Building QTAC for macOS $MACOS_ARCH ($CONFIG)"
    echo "Qt: $QT_ROOT"
    echo "FTDI D2XX: $FTD2XX_LIBRARY"
    echo "=========================================================="

    cmake -S "$REPO_ROOT" -B "$BUILD_DIR" \
        -G Ninja \
        -DCMAKE_PREFIX_PATH="$QT_ROOT" \
        -DCMAKE_OSX_ARCHITECTURES="$MACOS_ARCH" \
        -DCMAKE_BUILD_TYPE="$CONFIG" \
        -DFTD2XX_LIBRARY="$FTD2XX_LIBRARY"

    cmake --build "$BUILD_DIR" --parallel

    if [[ ! -x "$OUTPUT_BIN_DIR/TAC" ]]; then
        echo "ERROR: Build completed but $OUTPUT_BIN_DIR/TAC was not produced." >&2
        exit 1
    fi

    mkdir -p "$OUTPUT_LIB_DIR"
    if [[ "$FTD2XX_LIBRARY" != "$OUTPUT_FTDI" ]]; then
        cp -L "$FTD2XX_LIBRARY" "$OUTPUT_FTDI"
    fi
    CURRENT_FTDI_ID="$(otool -D "$OUTPUT_FTDI" | tail -n 1)"
    if [[ "$CURRENT_FTDI_ID" != '@rpath/libftd2xx.dylib' ]]; then
        install_name_tool -id '@rpath/libftd2xx.dylib' "$OUTPUT_FTDI"
    fi

    # Remove the FTDI source-tree search path and add a relative path to the
    # staged library. Qt's build-machine RPATH remains because Qt is not bundled.
    FTD2XX_SOURCE_DIR="$(cd "$(dirname "$FTD2XX_LIBRARY")" && pwd)"
    remove_ftdi_source_rpath() {
        local object="$1"
        local rpath
        while IFS= read -r rpath; do
            if [[ "$rpath" == "$FTD2XX_SOURCE_DIR" || "$rpath" == "$OUTPUT_LIB_DIR" ]]; then
                install_name_tool -delete_rpath "$rpath" "$object"
            fi
        done < <(otool -l "$object" | awk '$1 == "path" { print $2 }')
    }

    for binary in "$OUTPUT_BIN_DIR"/*; do
        [[ -f "$binary" ]] || continue
        file "$binary" | grep -q 'Mach-O' || continue
        remove_ftdi_source_rpath "$binary"

        if ! otool -l "$binary" | grep -Fq '@executable_path/../lib'; then
            install_name_tool -add_rpath '@executable_path/../lib' "$binary"
        fi

        while IFS= read -r linked_library; do
            if [[ "$linked_library" == *libftd2xx*.dylib && "$linked_library" != '@rpath/libftd2xx.dylib' ]]; then
                install_name_tool -change "$linked_library" '@rpath/libftd2xx.dylib' "$binary"
            fi
        done < <(otool -L "$binary" | awk 'NR > 1 { print $1 }')
    done

    for library in "$OUTPUT_LIB_DIR"/*.dylib; do
        [[ -f "$library" ]] || continue
        [[ "$library" == "$OUTPUT_FTDI" ]] && continue
        file "$library" | grep -q 'Mach-O' || continue
        remove_ftdi_source_rpath "$library"
        if ! otool -l "$library" | grep -Fq '@loader_path'; then
            install_name_tool -add_rpath '@loader_path' "$library"
        fi
        while IFS= read -r linked_library; do
            if [[ "$linked_library" == *libftd2xx*.dylib && "$linked_library" != '@rpath/libftd2xx.dylib' ]]; then
                install_name_tool -change "$linked_library" '@rpath/libftd2xx.dylib' "$library"
            fi
        done < <(otool -L "$library" | awk 'NR > 1 { print $1 }')
    done

    echo "Build complete: $OUTPUT_DIR"
    echo "TAC runtime dependencies:"
    otool -L "$OUTPUT_BIN_DIR/TAC" | grep -E 'libftd2xx|TACDev' || true
done
