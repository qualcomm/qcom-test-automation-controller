@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
echo vcvars loaded
set "PATH=C:\Qt\6.11.1\msvc2022_64\bin;%PATH%"
set QTBIN=C:\Qt\6.11.1\msvc2022_64\bin

cmake -S . -B build\Release -DCMAKE_PREFIX_PATH="%QTBIN%\.." -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=cl > build_out.txt 2>&1
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
