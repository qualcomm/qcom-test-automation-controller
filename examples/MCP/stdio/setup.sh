#!/bin/bash

# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../../.." && pwd)"

if [ ! -d "$REPO_ROOT/__Builds" ]; then
    echo "[1/3] Build not found. Running root build..."
    "$REPO_ROOT/build.sh"
else
    echo "[1/3] Build found at $REPO_ROOT/__Builds, skipping recompile."
fi

echo "[2/3] Installing TACDev Python library..."
pip install "$REPO_ROOT/interfaces/Python"

echo "[3/3] Installing MCP dependencies..."
pip install -r "$SCRIPT_DIR/requirements.txt"

echo ""
echo "Setup complete. Run the client directly (no server process needed):"
echo "  python examples/MCP/stdio/tacdev_mcp_client.py"
