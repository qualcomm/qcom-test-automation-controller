@echo off
set QTBIN=C:\Qt\6.11.1\msvc2022_64\bin
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
set "PATH=%QTBIN%;%PATH%"

if exist build rmdir /s /q build
if exist __Builds rmdir /s /q __Builds

cmake -S . -B build\Debug -DCMAKE_PREFIX_PATH="%QTBIN%\.." -DCMAKE_BUILD_TYPE=Debug
if errorlevel 1 goto :fail

cmake --build build\Debug
if errorlevel 1 goto :fail

cmake -S . -B build\Release -DCMAKE_PREFIX_PATH="%QTBIN%\.." -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 goto :fail

cmake --build build\Release
if errorlevel 1 goto :fail

echo BUILD SUCCEEDED
goto :end

:fail
echo BUILD FAILED
exit /b 1

:end
