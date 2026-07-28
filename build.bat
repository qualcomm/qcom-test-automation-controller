@REM  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
@REM  SPDX-License-Identifier: BSD-3-Clause

@echo off
setlocal EnableDelayedExpansion

@REM ---------------------------------------------------------------------------
@REM  Detect host architecture (native build only)
@REM ---------------------------------------------------------------------------
if /i "%PROCESSOR_ARCHITECTURE%"=="ARM64" (
    set ARCH=ARM64
    set EXPECTED_QT_PATH=msvc2022_arm64
    set VCVARS_SCRIPT=vcvarsarm64.bat
    set VS_COMPONENT=MSVC v143 - VS 2022 C++ ARM64 build tools
) else (
    set ARCH=x64
    set EXPECTED_QT_PATH=msvc2022_64
    set VCVARS_SCRIPT=vcvars64.bat
    set VS_COMPONENT=Desktop development with C++
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
set VS_INSTALL_DIR=

if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\%VCVARS_SCRIPT%" (
    call "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\%VCVARS_SCRIPT%"
    set VCVARS_FOUND=1
    set "VS_INSTALL_DIR=C:\Program Files\Microsoft Visual Studio\2022\Enterprise"
    echo VS2022 toolchain     : Enterprise [OK]
    goto :vcvars_done
)
if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\%VCVARS_SCRIPT%" (
    call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\%VCVARS_SCRIPT%"
    set VCVARS_FOUND=1
    set "VS_INSTALL_DIR=C:\Program Files\Microsoft Visual Studio\2022\Community"
    echo VS2022 toolchain     : Community [OK]
    goto :vcvars_done
)
if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\%VCVARS_SCRIPT%" (
    call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\%VCVARS_SCRIPT%"
    set VCVARS_FOUND=1
    set "VS_INSTALL_DIR=C:\Program Files\Microsoft Visual Studio\2022\Professional"
    echo VS2022 toolchain     : Professional [OK]
    goto :vcvars_done
)
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\%VCVARS_SCRIPT%" (
    call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\%VCVARS_SCRIPT%"
    set VCVARS_FOUND=1
    set "VS_INSTALL_DIR=C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools"
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
    echo          C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build
    echo          C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build
    echo          C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build
    echo          C:\Program Files ^(x86^)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build
    exit /b 1
)

@REM ---------------------------------------------------------------------------
@REM  Detection for TACDevInterop buildability (C#, .NET Framework 4.8, x64 only)
@REM ---------------------------------------------------------------------------
set BUILD_INTEROP=0
set NET48_FOUND=0
set MSBUILD_EXE=

if /i "%ARCH%"=="ARM64" (
    echo TACDevInterop       : skipped ^(not supported on ARM64^)
) else (
    set NET_RELEASE_HEX=0x0
    for /f "tokens=3" %%A in ('reg query "HKLM\SOFTWARE\Microsoft\NET Framework Setup\NDP\v4\Full" /v Release 2^>nul ^| findstr /i "Release"') do set NET_RELEASE_HEX=%%A
    set /A NET_RELEASE=!NET_RELEASE_HEX!
    if !NET_RELEASE! GEQ 528040 set NET48_FOUND=1
    
    if "!NET48_FOUND!"=="0" (
        echo TACDevInterop       : .NET Framework 4.8 not found on this machine [SKIP]
    ) else (
        where msbuild.exe >nul 2>nul
        if not errorlevel 1 (
            for /f "delims=" %%I in ('where msbuild.exe') do (
                if "!MSBUILD_EXE!"=="" set "MSBUILD_EXE=%%I"
            )
        )
        if "!MSBUILD_EXE!"=="" if not "%VS_INSTALL_DIR%"=="" (
            for /f "delims=" %%I in ('dir /s /b "%VS_INSTALL_DIR%\MSBuild\Current\Bin\MSBuild.exe" 2^>nul') do (
                if "!MSBUILD_EXE!"=="" set "MSBUILD_EXE=%%I"
            )
        )

        if "!MSBUILD_EXE!"=="" (
            echo TACDevInterop       : .NET Framework 4.8 found, but MSBuild not found [SKIP]
        ) else (
            echo TACDevInterop       : .NET Framework 4.8 + MSBuild found [OK]
            set BUILD_INTEROP=1
        )
    )
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

@REM ---------------------------------------------------------------------------
@REM  TACDevInterop.dll
@REM ---------------------------------------------------------------------------
if "%BUILD_INTEROP%"=="1" (
    echo.
    echo Building TACDevInterop ^(C#, x64, Release^)...
    "%MSBUILD_EXE%" "interfaces\C#\TACDevInterop\TACDevInterop.csproj" ^
    /p:Configuration=Release /p:Platform=x64 /nologo /verbosity:minimal
    if not "!ERRORLEVEL!"=="0" (
        echo WARNING: TACDevInterop build failed; continuing since it is an optional component.
    ) else (
        echo TACDevInterop.dll built -^> __Builds\x64\Release\bin\TACDevInterop.dll
    )
)

echo Check __Builds directory