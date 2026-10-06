#!/bin/bash

# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

#!/usr/bin/env bash
set -euo pipefail

PKG_NAME="${PKG_NAME:-qualcomm-qtac}"
VERSION="${VERSION:-1.0.0}"
ARCH="${ARCH:-linux-anycpu}"
MAINTAINER="${MAINTAINER:-Maintainer <maintainer@example.com>}"
DESCRIPTION="${DESCRIPTION:-Qualcomm QTAC tool package}"
INSTALL_PREFIX="${INSTALL_PREFIX:-/opt/qcom/QTAC}"
CONFIG_INSTALL_DIR="/var/lib/qcom/data/QTAC"

case "${1:-}" in
  -v|--version|version)
    echo "$PKG_NAME $VERSION"
    exit 0
    ;;
esac

OPTION_ZIP="${1:-}"

BASE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Source directories — can be overridden via env var for out-of-tree builds
SRC_DIR="${SRC_DIR:-$(realpath "$BASE_DIR/../../__Builds/Linux/Release")}"
CONFIG_SRC_DIR="$(realpath "$BASE_DIR/../../configurations")"
DOCS_SRC_DIR="$(realpath "$BASE_DIR/../../docs")"
EXAMPLES_SRC_DIR="$(realpath "$BASE_DIR/../../examples")"
PYTHON_SRC_DIR="$(realpath "$BASE_DIR/../../interfaces/Python")"
INTERFACES_SRC_DIR="$(realpath "$BASE_DIR/../../interfaces")"
UDEV_RULES_SRC_DIR="$(realpath "$BASE_DIR/../../udev-rules")"
PLUGINS_SRC_DIR="$SRC_DIR/plugins"

OUTPUT_DIR="${OUTPUT_DIR:-$BASE_DIR/build}"

DEB_ARCH="$ARCH"
case "$(echo "$ARCH" | tr '[:upper:]' '[:lower:]')" in
  linux-anycpu|anycpu|any|noarch|all)
    DEB_ARCH="all"
    ;;
  x86_64|x64|amd64)
    DEB_ARCH="amd64"
    ;;
  aarch64|arm64)
    DEB_ARCH="arm64"
    ;;
  armhf)
    DEB_ARCH="armhf"
    ;;
  i386|x86)
    DEB_ARCH="i386"
    ;;
esac

if ! command -v dpkg-deb >/dev/null 2>&1; then
    echo "ERROR: dpkg-deb not found." >&2
    exit 1
fi

missing=0

for d in bin lib; do
    if [ ! -d "$SRC_DIR/$d" ]; then
        echo "ERROR: Missing source directory: $SRC_DIR/$d" >&2
        missing=1
    elif [ -z "$(ls -A "$SRC_DIR/$d")" ]; then
        echo "ERROR: Source directory empty: $SRC_DIR/$d" >&2
        missing=1
    fi
done

if [ ! -d "$CONFIG_SRC_DIR" ]; then
    echo "ERROR: Missing configurations directory: $CONFIG_SRC_DIR" >&2
    missing=1
fi

if [ ! -d "$DOCS_SRC_DIR" ]; then
    echo "ERROR: Missing docs directory: $DOCS_SRC_DIR" >&2
    missing=1
fi

if [ ! -d "$EXAMPLES_SRC_DIR" ]; then
    echo "ERROR: Missing example directory: $EXAMPLES_SRC_DIR" >&2
    missing=1
fi

if [ ! -d "$PYTHON_SRC_DIR" ]; then
    echo "ERROR: Missing python directory: $PYTHON_SRC_DIR" >&2
    missing=1
fi

if [ ! -d "$INTERFACES_SRC_DIR" ]; then
    echo "ERROR: Missing interfaces directory: $INTERFACES_SRC_DIR" >&2
    missing=1
fi

if [ ! -d "$UDEV_RULES_SRC_DIR" ]; then
    echo "ERROR: Missing udev-rules directory: $UDEV_RULES_SRC_DIR" >&2
    missing=1
fi

if [ ! -d "$PLUGINS_SRC_DIR" ]; then
    echo "ERROR: Missing plugins directory: $PLUGINS_SRC_DIR" >&2
    missing=1
fi

if [ "$missing" -ne 0 ]; then
    exit 1
fi

WORKDIR="$(mktemp -d -t "${PKG_NAME}-build-XXXXXX")"
BUILDROOT="$WORKDIR/${PKG_NAME}_${VERSION}"

trap 'if [ "${NO_CLEANUP:-0}" -ne 1 ]; then rm -rf "$WORKDIR"; fi' EXIT

mkdir -p "$BUILDROOT/DEBIAN"
mkdir -p "$BUILDROOT$INSTALL_PREFIX/bin"
mkdir -p "$BUILDROOT$INSTALL_PREFIX/lib"
mkdir -p "$BUILDROOT$INSTALL_PREFIX/docs/QTAC"
mkdir -p "$BUILDROOT$INSTALL_PREFIX/examples"
mkdir -p "$BUILDROOT$INSTALL_PREFIX/python"
mkdir -p "$BUILDROOT$INSTALL_PREFIX/plugins"
mkdir -p "$BUILDROOT$INSTALL_PREFIX/utils"
mkdir -p "$BUILDROOT$CONFIG_INSTALL_DIR/configurations"
mkdir -p "$BUILDROOT$CONFIG_INSTALL_DIR/interfaces"
mkdir -p "$OUTPUT_DIR"

chmod 0755 "$BUILDROOT/DEBIAN"

echo "Copying bin..."
cp -a "$SRC_DIR/bin/." "$BUILDROOT$INSTALL_PREFIX/bin/"

echo "Copying lib..."
cp -a "$SRC_DIR/lib/." "$BUILDROOT$INSTALL_PREFIX/lib/"

echo "Copying configurations..."
cp -a "$CONFIG_SRC_DIR/." \
      "$BUILDROOT$CONFIG_INSTALL_DIR/configurations/"

echo "Copying docs..."
cp -a "$DOCS_SRC_DIR/." \
      "$BUILDROOT$INSTALL_PREFIX/docs/QTAC/"
	  
echo "Copying examples..."
cp -a "$EXAMPLES_SRC_DIR/." \
      "$BUILDROOT$INSTALL_PREFIX/examples/"
	  
echo "Copying Python..."
cp -a "$PYTHON_SRC_DIR/." \
      "$BUILDROOT$INSTALL_PREFIX/python/"

echo "Copying interfaces..."

if [ -d "$INTERFACES_SRC_DIR/C++/TACDev" ]; then
    mkdir -p "$BUILDROOT$CONFIG_INSTALL_DIR/interfaces/C++/TACDev"
    if ls "$INTERFACES_SRC_DIR/C++/TACDev/"*.h >/dev/null 2>&1; then
        cp -a "$INTERFACES_SRC_DIR/C++/TACDev/"*.h \
              "$BUILDROOT$CONFIG_INSTALL_DIR/interfaces/C++/TACDev/"
    else
        echo "WARNING: No C++ headers found in $INTERFACES_SRC_DIR/C++/TACDev" >&2
    fi
else
    echo "WARNING: Missing interfaces/C++/TACDev; C++ headers will be absent." >&2
fi

for lang in Python Java; do
    if [ -d "$INTERFACES_SRC_DIR/$lang" ]; then
        mkdir -p "$BUILDROOT$CONFIG_INSTALL_DIR/interfaces/$lang"
        cp -a "$INTERFACES_SRC_DIR/$lang/." \
              "$BUILDROOT$CONFIG_INSTALL_DIR/interfaces/$lang/"
    else
        echo "WARNING: Missing interfaces/$lang; $lang interface will be absent." >&2
    fi
done

echo "Copying plugins..."
cp -a "$PLUGINS_SRC_DIR/." \
      "$BUILDROOT$INSTALL_PREFIX/plugins/"
	  
echo "Copying udev rules..."
cp -a "$UDEV_RULES_SRC_DIR/." \
      "$BUILDROOT$INSTALL_PREFIX/utils/"

find "$BUILDROOT$INSTALL_PREFIX/bin" -type f -exec chmod 755 {} \;
find "$BUILDROOT$INSTALL_PREFIX/lib" -type f -exec chmod 755 {} \;

find "$BUILDROOT$INSTALL_PREFIX/docs/QTAC" -type f -exec chmod 644 {} \; || true
find "$BUILDROOT$INSTALL_PREFIX/examples" -type f -exec chmod 644 {} \; || true
find "$BUILDROOT$INSTALL_PREFIX/plugins" -type f -exec chmod 644 {} \; || true
find "$BUILDROOT$INSTALL_PREFIX/python" -type f -exec chmod 644 {} \; || true

find "$BUILDROOT$CONFIG_INSTALL_DIR/configurations" -type f -exec chmod 644 {} \; || true

find "$BUILDROOT$CONFIG_INSTALL_DIR/interfaces" -type f -exec chmod 644 {} \; || true
find "$BUILDROOT$CONFIG_INSTALL_DIR/interfaces" -type f -name "*.sh" -exec chmod 755 {} \; || true

###############################################################################
# Validate Interfaces Payload
###############################################################################

echo ""
echo "Validating interfaces payload..."

INTERFACES_STAGE_DIR="$BUILDROOT$CONFIG_INSTALL_DIR/interfaces"
interfaces_invalid=0

for rel in "C++/TACDev" "Python" "Java"; do

    staged="$INTERFACES_STAGE_DIR/$rel"

    if [ ! -d "$INTERFACES_SRC_DIR/$rel" ]; then
        echo "  SKIP  interfaces/$rel (not present in source tree)"
        continue
    fi

    if [ ! -d "$staged" ]; then
        echo "  ERROR interfaces/$rel was not staged at $staged" >&2
        interfaces_invalid=1
        continue
    fi

    count="$(find "$staged" -type f | wc -l | tr -d '[:space:]')"

    if [ "$count" -eq 0 ]; then
        echo "  ERROR interfaces/$rel staged but contains no files" >&2
        interfaces_invalid=1
        continue
    fi

    echo "  OK    interfaces/$rel ($count file(s))"

done

if [ "$interfaces_invalid" -ne 0 ]; then
    echo ""
    echo "ERROR: Interfaces payload validation failed." >&2
    echo "Expected interfaces under: $CONFIG_INSTALL_DIR/interfaces" >&2
    exit 1
fi

# C++ side must ship headers only (no sources / build files), matching the
# Windows installer which copies interfaces\C++\TACDev\*.h exclusively.
if [ -d "$INTERFACES_STAGE_DIR/C++/TACDev" ]; then

    unexpected="$(
        find "$INTERFACES_STAGE_DIR/C++/TACDev" \
            -type f \
            ! -name "*.h" \
            ! -name "*.hpp" \
            -printf '%p\n' 2>/dev/null || true
    )"

    if [ -n "$unexpected" ]; then
        echo ""
        echo "ERROR: Unexpected non-header files staged under interfaces/C++/TACDev:" >&2
        echo "$unexpected" >&2
        exit 1
    fi

    if ! find "$INTERFACES_STAGE_DIR/C++/TACDev" -name "*.h" | grep -q .; then
        echo ""
        echo "ERROR: No C++ headers staged under interfaces/C++/TACDev." >&2
        exit 1
    fi

fi

echo "Interfaces payload validation passed."

###############################################################################
# Validate Qt Plugin Dependencies
###############################################################################

echo ""
echo "Validating Qt plugin dependencies..."

find "$BUILDROOT$INSTALL_PREFIX/plugins" \
    -type f \
    -name "*.so*" | while read -r plugin
do

    missing=$(
		LD_LIBRARY_PATH="$BUILDROOT$INSTALL_PREFIX/lib" \
		ldd "$plugin" 2>/dev/null |
		grep "not found" |
		grep -v "libxcb-" ||
		true
	)

    if [ -n "$missing" ]; then

        echo ""
        echo "ERROR: Missing dependencies detected for:"
        echo "  $plugin"
        echo ""
        echo "$missing"
        exit 1

    fi

done

echo "Qt plugin dependency validation passed."

###############################################################################
# Verify Qt XCB Runtime Support
###############################################################################

if [ -d "$BUILDROOT$INSTALL_PREFIX/plugins/platforms" ]; then

    if ! find "$BUILDROOT$INSTALL_PREFIX/lib" \
            -name "libQt6XcbQpa.so*" | grep -q .; then

        echo ""
        echo "ERROR: libQt6XcbQpa was not packaged."
        echo "Expected to find:"
        echo "  libQt6XcbQpa.so"
        echo "  libQt6XcbQpa.so.6"
        echo "  libQt6XcbQpa.so.<version>"
        echo ""
        echo "Please fix the Qt deployment step."
        exit 1

    fi

fi


###############################################################################
# Fix RUNPATH for deployed binaries
###############################################################################

if command -v patchelf >/dev/null 2>&1; then

    echo "Fixing RUNPATH..."

    find "$BUILDROOT$INSTALL_PREFIX/bin" \
        -type f \
        -executable | while read -r exe
    do
        echo "Patching: $exe"

        patchelf \
            --set-rpath '$ORIGIN/../lib' \
            "$exe"
    done

else

    echo "WARNING: patchelf not found."
    echo "Install it using:"
    echo "  sudo apt install patchelf"

fi

chown -R root:root "$BUILDROOT" 2>/dev/null || true

cat > "$BUILDROOT/DEBIAN/control" <<EOF
Package: $PKG_NAME
Version: $VERSION
Section: utils
Priority: optional
Architecture: $DEB_ARCH
Maintainer: $MAINTAINER
Depends: bash, coreutils, libxcb-cursor0, libxcb-icccm4, libxcb-util1, libxcb-image0, libxcb-keysyms1, libxcb-render-util0
Description: $DESCRIPTION
EOF

chmod 0644 "$BUILDROOT/DEBIAN/control"

cat > "$BUILDROOT/DEBIAN/postinst" <<EOF
#!/usr/bin/env bash
set -e

INSTALL_PREFIX="$INSTALL_PREFIX"
CONFIG_DIR="$CONFIG_INSTALL_DIR/configurations"
LOG_FILE="\$INSTALL_PREFIX/qtac_install.log"

export LD_LIBRARY_PATH="\$INSTALL_PREFIX/lib:\${LD_LIBRARY_PATH:-}"

mkdir -p "\$INSTALL_PREFIX" || true

touch "\$LOG_FILE" || true
chmod 0644 "\$LOG_FILE" || true

echo "" >> "\$LOG_FILE"
echo "==============================================================" >> "\$LOG_FILE"
echo "[QTAC] Installation started: \$(date)" >> "\$LOG_FILE"
echo "==============================================================" >> "\$LOG_FILE"

chmod -R 0755 "\$INSTALL_PREFIX" || true
chmod -R 0755 "$CONFIG_INSTALL_DIR" || true

chown -R root:root "\$INSTALL_PREFIX" || true
chown -R root:root "$CONFIG_INSTALL_DIR" || true

echo "[QTAC] Executing UpdateDeviceList..." >> "\$LOG_FILE"
echo "[QTAC] Command: \$INSTALL_PREFIX/bin/UpdateDeviceList dir=\$CONFIG_DIR" >> "\$LOG_FILE"

if [ -x "\$INSTALL_PREFIX/bin/UpdateDeviceList" ]; then

    if "\$INSTALL_PREFIX/bin/UpdateDeviceList" \
        dir="\$CONFIG_DIR" \
        >> "\$LOG_FILE" 2>&1
    then

        echo "[QTAC] UpdateDeviceList completed successfully." >> "\$LOG_FILE"

    else

        RET=\$?

        echo "[QTAC] WARNING: UpdateDeviceList failed with return code \$RET" \
            >> "\$LOG_FILE"

    fi

else

    echo "[QTAC] ERROR: UpdateDeviceList not found or not executable." >> "\$LOG_FILE"

fi

###############################################################################
# Ensure libxcb-cursor0 is installed
###############################################################################

if ! ldconfig -p 2>/dev/null | grep -q "libxcb-cursor.so.0"; then

    echo "[QTAC] libxcb-cursor0 not found. Installing..." \
        >> "\$LOG_FILE"

    apt-get update \
        >> "\$LOG_FILE" 2>&1 || true

    DEBIAN_FRONTEND=noninteractive \
	apt-get install -y \
		libxcb-cursor0 \
		libxcb-icccm4 \
		libxcb-util1 \
		libxcb-image0 \
		libxcb-keysyms1 \
		libxcb-render-util0 \
    >> "\$LOG_FILE" 2>&1 || true

    ldconfig >/dev/null 2>&1 || true

    if ldconfig -p 2>/dev/null | grep -q "libxcb-cursor.so.0"; then

        echo "[QTAC] Successfully installed libxcb-cursor0." \
            >> "\$LOG_FILE"

    else

        echo "[QTAC] WARNING: Failed to install libxcb-cursor0." \
            >> "\$LOG_FILE"

    fi

fi

echo "[QTAC] Installing udev rules..." >> "\$LOG_FILE"

if [ -f "\$INSTALL_PREFIX/utils/99-QTAC-USB.rules" ]; then

    cp "\$INSTALL_PREFIX/utils/99-QTAC-USB.rules" \
       /etc/udev/rules.d/

    udevadm control --reload

    echo "[QTAC] udev rules installed successfully." >> "\$LOG_FILE"

else

    echo "[QTAC] ERROR: 99-QTAC-USB.rules not found." >> "\$LOG_FILE"

fi

echo "[QTAC] Installation complete." >> "\$LOG_FILE"

exit 0
EOF

chmod 0755 "$BUILDROOT/DEBIAN/postinst"

cat > "$BUILDROOT/DEBIAN/prerm" <<'EOF'
#!/usr/bin/env bash
set -e

LOG_FILE="/opt/qcom/QTAC/qtac_install.log"

if [ -f "$LOG_FILE" ]; then
    echo "" >> "$LOG_FILE" 2>&1
    echo "==============================================================" >> "$LOG_FILE" 2>&1
    echo "[QTAC] Uninstall started: $(date)" >> "$LOG_FILE" 2>&1
    echo "==============================================================" >> "$LOG_FILE" 2>&1
fi

exit 0
EOF

chmod 0755 "$BUILDROOT/DEBIAN/prerm"

cat > "$BUILDROOT/DEBIAN/postrm" <<'EOF'
#!/usr/bin/env bash
set -e

LOG_FILE="/opt/qcom/QTAC/qtac_install.log"

# Remove installation log
rm -f "$LOG_FILE"

# Remove udev rule
rm -f /etc/udev/rules.d/99-QTAC-USB.rules

# Reload udev
udevadm control --reload 2>/dev/null || true

# Remove QTAC install directory if empty
rmdir /opt/qcom/QTAC 2>/dev/null || true

# Remove configuration directory if empty
rmdir /var/lib/qcom/data/QTAC 2>/dev/null || true

# Remove parent directory if empty
rmdir /var/lib/qcom/data 2>/dev/null || true

exit 0
EOF

chmod 0755 "$BUILDROOT/DEBIAN/postrm"

OUTPUT_DEB="$OUTPUT_DIR/${PKG_NAME}_${VERSION}_${ARCH}.deb"

echo "Building package -> $OUTPUT_DEB"

if dpkg-deb --help 2>&1 | grep -q -- '--root-owner-group'; then
    dpkg-deb --build --root-owner-group "$BUILDROOT" "$OUTPUT_DEB"
else
    dpkg-deb --build "$BUILDROOT" "$OUTPUT_DEB"
fi

echo "Successfully built: $OUTPUT_DEB"

###############################################################################
# Verify Generated Package Contents
###############################################################################

echo ""
echo "Verifying generated package contents..."

DEB_CONTENTS="$WORKDIR/deb-contents.txt"
dpkg-deb -c "$OUTPUT_DEB" > "$DEB_CONTENTS"

pkg_invalid=0

# Strip the leading '.' dpkg-deb prints for absolute paths.
expected_paths=(
    "$INSTALL_PREFIX/bin/"
    "$INSTALL_PREFIX/lib/"
    "$CONFIG_INSTALL_DIR/configurations/"
    "$CONFIG_INSTALL_DIR/interfaces/"
)

for lang_rel in "C++/TACDev" "Python" "Java"; do
    if [ -d "$INTERFACES_SRC_DIR/$lang_rel" ]; then
        expected_paths+=("$CONFIG_INSTALL_DIR/interfaces/$lang_rel/")
    fi
done

for p in "${expected_paths[@]}"; do

    if grep -q -F " .$p" "$DEB_CONTENTS"; then
        echo "  OK    $p"
    else
        echo "  ERROR $p missing from $OUTPUT_DEB" >&2
        pkg_invalid=1
    fi

done

if [ "$pkg_invalid" -ne 0 ]; then
    echo ""
    echo "ERROR: Generated package is missing expected content." >&2
    echo "Full package listing:" >&2
    cat "$DEB_CONTENTS" >&2
    exit 1
fi

interfaces_in_deb="$(grep -c -F " .$CONFIG_INSTALL_DIR/interfaces/" "$DEB_CONTENTS" || true)"
echo "Package content verification passed ($interfaces_in_deb interfaces entries)."

ZIP_FOLDER="${PKG_NAME}_${VERSION}_${ARCH}"

if [ "${OPTION_ZIP:-}" = "zip" ]; then
    mkdir -p "$ZIP_FOLDER"

    cp "$OUTPUT_DEB" "$ZIP_FOLDER/"

    [ -f "./README.md" ] && \
        cp "./README.md" "$ZIP_FOLDER/"

    [ -f "./ReleaseNotes.txt" ] && \
        cp "./ReleaseNotes.txt" "$ZIP_FOLDER/"

    zip -r "${ZIP_FOLDER}.zip" "$ZIP_FOLDER"

    rm -rf "$ZIP_FOLDER"

    echo "Archive created: ${ZIP_FOLDER}.zip"
fi
