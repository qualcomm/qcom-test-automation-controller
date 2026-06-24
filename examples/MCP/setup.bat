@REM  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
@REM  SPDX-License-Identifier: BSD-3-Clause

@echo off

@REM Resolve repo root (two levels up from examples\MCP)
set SCRIPT_DIR=%~dp0
pushd "%SCRIPT_DIR%\..\.."
set REPO_ROOT=%CD%
popd

@REM Step 1: build the project
echo [1/3] Building QTAC...
call "%REPO_ROOT%\build.bat" %1
if errorlevel 1 exit /b 1

@REM Step 2: install TACDev Python library
echo [2/3] Installing TACDev Python library...
pip install "%REPO_ROOT%\interfaces\Python"
if errorlevel 1 exit /b 1

@REM Step 3: install MCP dependencies
echo [3/3] Installing MCP dependencies...
pip install -r "%SCRIPT_DIR%requirements.txt"
if errorlevel 1 exit /b 1

echo.
echo Setup complete. Run the MCP server from the repo root:
echo   python examples\MCP\tacdev_mcp_server.py
