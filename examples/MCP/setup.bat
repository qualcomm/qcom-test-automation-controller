@REM  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
@REM  SPDX-License-Identifier: BSD-3-Clause

@echo off

set SCRIPT_DIR=%~dp0
pushd "%SCRIPT_DIR%\..\.."
set REPO_ROOT=%CD%
popd

set ARCH=%1
if "%ARCH%"=="" (
    if "%PROCESSOR_ARCHITECTURE%"=="ARM64" (
        set ARCH=ARM64
    ) else (
        set ARCH=x64
    )
)

@REM On ARM64, the cryptography package (required by fastmcp) must be compiled
@REM from source. This requires OpenSSL development headers. Check early.
if /i "%ARCH%"=="ARM64" (
    if "%OPENSSL_DIR%"=="" set OPENSSL_DIR=C:\Program Files\OpenSSL-Win64-ARM

    if not exist "%OPENSSL_DIR%\include" (
        echo OpenSSL not found. Downloading and installing...
        set OPENSSL_MSI=%TEMP%\Win64ARMOpenSSL-4_0_1.msi
        curl -L -o "%OPENSSL_MSI%" https://slproweb.com/download/Win64ARMOpenSSL-4_0_1.msi
        if errorlevel 1 (
            echo.
            echo ERROR: Failed to download OpenSSL installer.
            echo        Check your internet connection and try again, or download manually:
            echo          https://slproweb.com/download/Win64ARMOpenSSL-4_0_1.msi
            echo        Install it, then re-run setup.bat
            echo.
            exit /b 1
        )
        msiexec /i "%OPENSSL_MSI%" /quiet /norestart INSTALLDIR="%OPENSSL_DIR%"
        if errorlevel 1 (
            echo.
            echo ERROR: OpenSSL installation failed.
            echo        Try installing manually from:
            echo          https://slproweb.com/download/Win64ARMOpenSSL-4_0_1.msi
            echo        Then re-run setup.bat
            echo.
            exit /b 1
        )
        echo OpenSSL installed to %OPENSSL_DIR%
    )

    if not exist "%OPENSSL_DIR%\include" (
        echo.
        echo ERROR: OpenSSL headers still not found at %OPENSSL_DIR%\include
        echo        Installation may have used a different path.
        echo        Set OPENSSL_DIR manually and re-run:
        echo          setx OPENSSL_DIR "C:\Program Files\OpenSSL-Win64-ARM"
        echo.
        exit /b 1
    )

    set OPENSSL_LIB_DIR=%OPENSSL_DIR%\lib\VC\arm64\MD
    if not exist "%OPENSSL_LIB_DIR%" (
        echo.
        echo ERROR: OpenSSL lib directory not found at %OPENSSL_LIB_DIR%
        echo        Expected layout from the full ARM64 installer. Try reinstalling from:
        echo          https://slproweb.com/download/Win64ARMOpenSSL-4_0_1.msi
        echo.
        exit /b 1
    )

    echo OpenSSL              : %OPENSSL_DIR% [OK]
    echo OpenSSL libs         : %OPENSSL_LIB_DIR% [OK]
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
