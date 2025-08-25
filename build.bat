@echo off
REM Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries. 
REM  
REM Redistribution and use in source and binary forms, with or without
REM modification, are permitted (subject to the limitations in the
REM disclaimer below) provided that the following conditions are met:
REM  
REM 	* Redistributions of source code must retain the above copyright
REM 	  notice, this list of conditions and the following disclaimer.
REM  
REM 	* Redistributions in binary form must reproduce the above
REM 	  copyright notice, this list of conditions and the following
REM 	  disclaimer in the documentation and/or other materials provided
REM 	  with the distribution.
REM  
REM 	* Neither the name of Qualcomm Technologies, Inc. nor the names of its
REM 	  contributors may be used to endorse or promote products derived
REM 	  from this software without specific prior written permission.
REM  
REM NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
REM GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
REM HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
REM WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
REM MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
REM IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
REM ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
REM DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
REM GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
REM INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
REM IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
REM OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
REM IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

REM Author: Biswajit Roy <biswroy@qti.qualcomm.com>


setlocal

REM Set default build type if not specified
if "%1"=="" (
    set BUILD_TYPE=Release
) else (
    set BUILD_TYPE=%1
)

echo Building QTAC Workspace with CMake...
echo Build Type: %BUILD_TYPE%

REM Check for Qt environment variables
if defined QTBIN (
    echo Using Qt from QTBIN: %QTBIN%
) else if defined QTDIR (
    echo Using Qt from QTDIR: %QTDIR%
) else (
    echo WARNING: QTDIR/QTBIN environment variables not set
    echo Please configure as documented in README.md
)

REM Create build directory
if not exist "build" mkdir build
cd build

REM Configure with CMake (Qt paths auto-detected via environment variables)
echo Configuring project...
cmake -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ..
if errorlevel 1 (
    echo Configuration failed!
    echo Please check Qt installation and environment variables
    exit /b 1
)

REM Build the project
echo Building project...
cmake --build . --config %BUILD_TYPE% --parallel
if errorlevel 1 (
    echo Build failed!
    exit /b 1
)

echo Build completed successfully!
echo Binaries: build\bin\
echo Libraries: build\lib\

endlocal
