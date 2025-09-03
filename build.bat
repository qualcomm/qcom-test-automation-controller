@REM  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries. 

@REM  Redistribution and use in source and binary forms, with or without
@REM  modification, are permitted (subject to the limitations in the
@REM  disclaimer below) provided that the following conditions are met:
     
@REM      * Redistributions of source code must retain the above copyright
@REM          notice, this list of conditions and the following disclaimer.
     
@REM      * Redistributions in binary form must reproduce the above
@REM          copyright notice, this list of conditions and the following
@REM          disclaimer in the documentation and/or other materials provided
@REM          with the distribution.
     
@REM      * Neither the name of Qualcomm Technologies, Inc. nor the names of its
@REM          contributors may be used to endorse or promote products derived
@REM          from this software without specific prior written permission.
     
@REM  NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
@REM  GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
@REM  HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
@REM  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
@REM  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
@REM  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
@REM  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
@REM  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
@REM  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
@REM  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
@REM  IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
@REM  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
@REM  IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

@REM  Author: Biswajit Roy (biswroy@qti.qualcomm.com)

@echo off

if "%QTBIN%"=="" (
    echo Set QTBIN to the bin directory of the Qt installed location
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
