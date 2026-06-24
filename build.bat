@REM  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
@REM  SPDX-License-Identifier: BSD-3-Clause

@echo off

@REM ---------------------------------------------------------------------------
@REM  Detect target architecture
@REM ---------------------------------------------------------------------------
set ARCH=%1
if "%ARCH%"=="" (
    if "%PROCESSOR_ARCHITECTURE%"=="ARM64" (
        set ARCH=ARM64
    ) else (
        set ARCH=x64
    )
)

if /i "%ARCH%"=="x64" (
    set EXPECTED_QT_PATH=msvc2022_64
    set VCVARS_SCRIPT=vcvars64.bat
    set VS_COMPONENT=Desktop development with C++
) else if /i "%ARCH%"=="ARM64" (
    set EXPECTED_QT_PATH=msvc2022_arm64
    set VCVARS_SCRIPT=vcvarsarm64.bat
    set VS_COMPONENT=MSVC v143 - VS 2022 C++ ARM64 build tools
) else (
    echo ERROR: Unsupported architecture '%ARCH%'.
    echo        Usage:
    echo          build.bat        - auto-detect from host machine ^(current: %PROCESSOR_ARCHITECTURE%^)
    echo          build.bat x64    - build for x64
    echo          build.bat ARM64  - build for ARM64
    exit /b 1
)

echo Architecture        : %ARCH%

@REM ---------------------------------------------------------------------------
@REM  Validate QTBIN
@REM ---------------------------------------------------------------------------
if "%QTBIN%"=="" (
    echo.
    echo ERROR: QTBIN is not set.
    echo        QTBIN must point to the Qt bin directory for your target architecture ^(%ARCH%^).
    echo        Run the following command and then open a new command prompt:
    if /i "%ARCH%"=="x64" (
        echo          setx QTBIN "C:\Qt\^<version^>\msvc2022_64\bin"
    ) else (
        echo          setx QTBIN "C:\Qt\^<version^>\msvc2022_arm64\bin"
    )
    exit /b 1
)

if not exist "%QTBIN%" (
    echo.
    echo ERROR: QTBIN directory does not exist: %QTBIN%
    echo        Qt does not appear to be installed at this path.
    echo        Install Qt 6.9+ via the Qt Online Installer ^(https://www.qt.io/download-qt-installer-oss^)
    echo        and include the MSVC 2022 %ARCH% component, then update QTBIN.
    exit /b 1
)

echo %QTBIN% | findstr /i "%EXPECTED_QT_PATH%" >nul
if errorlevel 1 (
    echo.
    echo ERROR: QTBIN points to the wrong Qt architecture for a %ARCH% build.
    echo        QTBIN is currently: %QTBIN%
    if /i "%ARCH%"=="x64" (
        echo        An x64 build requires the Qt MSVC 2022 64-bit component. QTBIN must contain 'msvc2022_64', e.g.:
        echo          setx QTBIN "C:\Qt\^<version^>\msvc2022_64\bin"
    ) else (
        echo        An ARM64 build requires the Qt MSVC 2022 ARM64 component. QTBIN must contain 'msvc2022_arm64', e.g.:
        echo          setx QTBIN "C:\Qt\^<version^>\msvc2022_arm64\bin"
        echo        If you have not installed the ARM64 Qt component, open Qt Online Installer, select Modify,
        echo        and add 'MSVC 2022 ARM64' under Qt ^<version^>.
    )
    exit /b 1
)

echo QTBIN               : %QTBIN% [OK]

@REM ---------------------------------------------------------------------------
@REM  Locate and call VS2022 vcvars
@REM ---------------------------------------------------------------------------
set VCVARS_FOUND=0

if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\%VCVARS_SCRIPT%" (
    call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\%VCVARS_SCRIPT%"
    set VCVARS_FOUND=1
    echo VS2022 toolchain     : Community [OK]
    goto :vcvars_done
)
if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\%VCVARS_SCRIPT%" (
    call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\%VCVARS_SCRIPT%"
    set VCVARS_FOUND=1
    echo VS2022 toolchain     : Professional [OK]
    goto :vcvars_done
)
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\%VCVARS_SCRIPT%" (
    call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\%VCVARS_SCRIPT%"
    set VCVARS_FOUND=1
    echo VS2022 toolchain     : BuildTools [OK]
    goto :vcvars_done
)

:vcvars_done
if "%VCVARS_FOUND%"=="0" (
    echo.
    echo ERROR: Visual Studio 2022 %ARCH% build tools not found ^(%VCVARS_SCRIPT%^).
    echo        Open Visual Studio Installer, click Modify on your VS2022 installation,
    echo        go to Individual Components, and install:
    echo          '%VS_COMPONENT%'
    echo        Searched in:
    echo          C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build
    echo          C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build
    echo          C:\Program Files ^(x86^)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build
    exit /b 1
)

@REM ---------------------------------------------------------------------------
@REM  Build
@REM ---------------------------------------------------------------------------
set "PATH=%QTBIN%;%PATH%"

if exist build rmdir /s /q build
if exist __Builds rmdir /s /q __Builds

cmake -S . -B build\Debug -DCMAKE_PREFIX_PATH="%QTBIN%\.." ^
    -DCMAKE_COLOR_DIAGNOSTICS=ON ^
    -DCMAKE_GENERATOR=Ninja ^
    -DCMAKE_BUILD_TYPE=Debug ^
    -DCMAKE_CXX_FLAGS_INIT=-DQT_QML_DEBUG

cmake --build build\Debug

cmake -S . -B build\Release -DCMAKE_PREFIX_PATH="%QTBIN%\.." ^
    -DCMAKE_COLOR_DIAGNOSTICS=ON ^
    -DCMAKE_GENERATOR=Ninja ^
    -DCMAKE_BUILD_TYPE=Release

cmake --build build\Release

echo Check __Builds directory
