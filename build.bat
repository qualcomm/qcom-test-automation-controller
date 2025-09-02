@echo off

if "%QTBIN%"=="" (
    echo Set QTBIN first
    exit /b 1
)

call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

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
