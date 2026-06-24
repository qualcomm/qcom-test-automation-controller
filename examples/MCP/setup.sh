#!/bin/bash

# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

# Step 1: build the project
echo "[1/3] Building QTAC..."
"$REPO_ROOT/build.sh"

# Step 2: install TACDev Python library
echo "[2/3] Installing TACDev Python library..."
pip install "$REPO_ROOT/interfaces/Python"

# Step 3: install MCP dependencies
echo "[3/3] Installing MCP dependencies..."
pip install -r "$SCRIPT_DIR/requirements.txt"

echo ""
echo "Setup complete. Run the MCP server from the repo root:"
echo "  python examples/MCP/tacdev_mcp_server.py"
