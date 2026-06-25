@REM  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
@REM  SPDX-License-Identifier: BSD-3-Clause

@echo off

set SCRIPT_DIR=%~dp0
pushd "%SCRIPT_DIR%\..\.."
set REPO_ROOT=%CD%
popd

set ARCH=%1
if "%ARCH%"=="" set ARCH=x64

@REM On ARM64, the cryptography package (required by fastmcp) must be compiled
@REM from source. This requires OpenSSL development headers. Check early.
if /i "%ARCH%"=="ARM64" (
    if "%OPENSSL_DIR%"=="" (
        echo.
        echo ERROR: OPENSSL_DIR is not set.
        echo        ARM64 builds require OpenSSL development headers to compile the
        echo        cryptography package. Install OpenSSL ARM64 and set OPENSSL_DIR:
        echo.
        echo          1. Download the ARM64 OpenSSL installer from https://slproweb.com/products/Win32OpenSSL.html
        echo          2. Install it, then run:
        echo               setx OPENSSL_DIR "C:\Program Files\OpenSSL-ARM64"
        echo          3. Open a new command prompt and re-run setup.bat ARM64
        echo.
        exit /b 1
    )
    echo OpenSSL              : %OPENSSL_DIR% [OK]
)

set BUILD_DIR=%REPO_ROOT%\__Builds

if not exist "%BUILD_DIR%" (
    echo [1/3] Build not found. Running root build...
    call "%REPO_ROOT%\build.bat" %ARCH%
    if errorlevel 1 exit /b 1
) else (
    echo [1/3] Build found at %BUILD_DIR%, skipping recompile.
)

echo [2/3] Installing TACDev Python library...
pip install "%REPO_ROOT%\interfaces\Python"
if errorlevel 1 exit /b 1

echo [3/3] Installing MCP dependencies...
pip install -r "%SCRIPT_DIR%requirements.txt"
if errorlevel 1 exit /b 1

echo.
echo Setup complete. Start the MCP server:
echo   python examples\MCP\tacdev_mcp_server.py
