@echo off
setlocal

:: Locate vcvars64 via vswhere (prefers newest VS install)
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" (
    echo vswhere.exe not found. Is Visual Studio 2022 installed?
    exit /b 1
)
for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualCpp.Tools.HostX64.TargetX64 -property installationPath`) do set "VS_INSTALLPATH=%%i"
if not defined VS_INSTALLPATH (
    echo No VS2022 install with VC++ x64 tools found.
    exit /b 1
)
call "%VS_INSTALLPATH%\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
echo vcvars loaded

:: Locate Qt — honor QTDIR env var, else use default install location
if defined QTDIR (
    set "QTBIN=%QTDIR%\bin"
) else (
    :: Search for msvc2022_64 under C:\Qt
    for /f "usebackq tokens=*" %%i in (`dir /b /s /ad "C:\Qt\msvc2022_64" 2^>nul`) do (
        if exist "%%i\bin\qmake.exe" set "QTROOT=%%i" & goto :qt_found
    )
    echo Qt not found. Set QTDIR or install Qt to C:\Qt\
    exit /b 1
    :qt_found
    set "QTBIN=%QTROOT%\bin"
)
set "PATH=%QTBIN%;%PATH%"

cmake -S source -B build\Release -DCMAKE_PREFIX_PATH="%QTROOT%" -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=cl > build_out.txt 2>&1
if errorlevel 1 (
    echo CMAKE CONFIGURE FAILED
    type build_out.txt
    exit /b 1
)

cmake --build build\Release --target qtac-app TACDev test_tacdev_api test_hardware_psoc >> build_out.txt 2>&1
if errorlevel 1 (
    echo CMAKE BUILD FAILED
    type build_out.txt
    exit /b 1
)

echo BUILD SUCCESS
type build_out.txt
