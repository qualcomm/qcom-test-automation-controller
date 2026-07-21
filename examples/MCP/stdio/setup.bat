@REM  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
@REM  SPDX-License-Identifier: BSD-3-Clause

@echo off
setlocal enabledelayedexpansion

set SCRIPT_DIR=%~dp0
pushd "%SCRIPT_DIR%\..\..\..\"
set REPO_ROOT=%CD%
popd

set ARCH=%1
if "%ARCH%"=="" (
    if "%PROCESSOR_ARCHITECTURE%"=="ARM64" (set ARCH=ARM64) else (set ARCH=x64)
)

if /i "%ARCH%"=="ARM64" (
    if "%OPENSSL_DIR%"=="" set OPENSSL_DIR=C:\Program Files\OpenSSL-Win64-ARM
)

if /i "%ARCH%"=="ARM64" if not exist "!OPENSSL_DIR!\include" (
    echo OpenSSL not found. Downloading and installing...
    curl -L -o "%TEMP%\Win64ARMOpenSSL.msi" https://slproweb.com/download/Win64ARMOpenSSL-4_0_1.msi
    if errorlevel 1 (
        echo.
        echo ERROR: Download failed. Install manually from:
        echo          https://slproweb.com/download/Win64ARMOpenSSL-4_0_1.msi
        echo        Then re-run setup.bat
        echo.
        exit /b 1
    )
    msiexec /i "%TEMP%\Win64ARMOpenSSL.msi" /quiet /norestart INSTALLDIR="!OPENSSL_DIR!"
    if errorlevel 1 (
        echo.
        echo ERROR: OpenSSL installation failed. Install manually from:
        echo          https://slproweb.com/download/Win64ARMOpenSSL-4_0_1.msi
        echo        Then re-run setup.bat
        echo.
        exit /b 1
    )
    echo OpenSSL installed to !OPENSSL_DIR!
)

if /i "%ARCH%"=="ARM64" (
    if not exist "!OPENSSL_DIR!\include" (
        echo.
        echo ERROR: OpenSSL headers not found at !OPENSSL_DIR!\include
        echo        Set OPENSSL_DIR to the full installer path and re-run:
        echo          setx OPENSSL_DIR "C:\Program Files\OpenSSL-Win64-ARM"
        echo.
        exit /b 1
    )
    if not exist "!OPENSSL_DIR!\lib\VC\arm64\MD" (
        echo.
        echo ERROR: OpenSSL libs not found at !OPENSSL_DIR!\lib\VC\arm64\MD
        echo        Reinstall using the full installer ^(not Light^):
        echo          https://slproweb.com/download/Win64ARMOpenSSL-4_0_1.msi
        echo.
        exit /b 1
    )
    set OPENSSL_LIB_DIR=!OPENSSL_DIR!\lib\VC\arm64\MD
    echo OpenSSL              : !OPENSSL_DIR! [OK]
    echo OpenSSL libs         : !OPENSSL_LIB_DIR! [OK]
)

set BUILD_DIR=%REPO_ROOT%\__Builds
if not exist "%BUILD_DIR%" (
    echo [1/3] Build not found. Running root build...
    call "%REPO_ROOT%\build.bat"
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
echo Setup complete. Run the client directly (no server process needed):
echo   python examples\MCP\stdio\tacdev_mcp_client.py
