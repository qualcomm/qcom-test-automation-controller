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

# Source directories
SRC_DIR="$BASE_DIR"
CONFIG_SRC_DIR="$(realpath "$BASE_DIR/../../../configurations")"
DOCS_SRC_DIR="$(realpath "$BASE_DIR/../../../docs")"
EXAMPLES_SRC_DIR="$(realpath "$BASE_DIR/../../../examples")"
PYTHON_SRC_DIR="$(realpath "$BASE_DIR/../../../interfaces/Python")"
UDEV_RULES_SRC_DIR="$(realpath "$BASE_DIR/../../../udev-rules")"
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
mkdir -p "$BUILDROOT$INSTALL_PREFIX/docs"
mkdir -p "$BUILDROOT$INSTALL_PREFIX/examples"
mkdir -p "$BUILDROOT$INSTALL_PREFIX/python"
mkdir -p "$BUILDROOT$INSTALL_PREFIX/plugins"
mkdir -p "$BUILDROOT$INSTALL_PREFIX/utils"
mkdir -p "$BUILDROOT$CONFIG_INSTALL_DIR/configurations"
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
      "$BUILDROOT$INSTALL_PREFIX/docs/"
	  
echo "Copying examples..."
cp -a "$EXAMPLES_SRC_DIR/." \
      "$BUILDROOT$INSTALL_PREFIX/examples/"
	  
echo "Copying Python..."
cp -a "$PYTHON_SRC_DIR/." \
      "$BUILDROOT$INSTALL_PREFIX/python/"

echo "Copying plugins..."
cp -a "$PLUGINS_SRC_DIR/." \
      "$BUILDROOT$INSTALL_PREFIX/plugins/"
	  
echo "Copying udev rules..."
cp -a "$UDEV_RULES_SRC_DIR/." \
      "$BUILDROOT$INSTALL_PREFIX/utils/"

find "$BUILDROOT$INSTALL_PREFIX/bin" -type f -exec chmod 755 {} \;
find "$BUILDROOT$INSTALL_PREFIX/lib" -type f -exec chmod 755 {} \;

find "$BUILDROOT$INSTALL_PREFIX/docs" -type f -exec chmod 644 {} \; || true
find "$BUILDROOT$INSTALL_PREFIX/examples" -type f -exec chmod 644 {} \; || true
find "$BUILDROOT$INSTALL_PREFIX/plugins" -type f -exec chmod 644 {} \; || true
find "$BUILDROOT$INSTALL_PREFIX/python" -type f -exec chmod 644 {} \; || true

find "$BUILDROOT$CONFIG_INSTALL_DIR/configurations" -type f -exec chmod 644 {} \; || true

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
Depends: bash, coreutils
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