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

REM Author: Biswajit Roy biswroy@qti.qualcomm.com

@echo off
setlocal

REM Check if the current directory is 'third-party' and navigate up if so
for %%i in ("%cd%") do set "CURRENT_DIR_NAME=%%~nxi"
if /I "%CURRENT_DIR_NAME%"=="third-party" (
    echo Current directory is "third-party". Navigating up to the project root.
    pushd ..
)

REM Set paths relative to project root
set "ROOT_DIR=%cd%"
set "THIRD_PARTY_DIR=%ROOT_DIR%\third-party"
set "ZIP_FILE=%THIRD_PARTY_DIR%\CDM-v2.12.36.20-WHQL-Certified.zip"
set "TEMP_DIR=%THIRD_PARTY_DIR%\ftdi_temp"
set "DEBUG_BIN_DIR=%ROOT_DIR%\__Builds\x64\Debug\bin"
set "RELEASE_BIN_DIR=%ROOT_DIR%\__Builds\x64\Release\bin"
set "DEBUG_LIB_DIR=%ROOT_DIR%\__Builds\x64\Debug\lib"
set "RELEASE_LIB_DIR=%ROOT_DIR%\__Builds\x64\Release\lib"
set "FTDI_URL=https://ftdichip.com/wp-content/uploads/2025/03/CDM-v2.12.36.20-WHQL-Certified.zip"

REM Check if DLLs and LIBs already exist
if exist "%DEBUG_BIN_DIR%\ftd2xx.dll" if exist "%RELEASE_BIN_DIR%\ftd2xx.dll" if exist "%DEBUG_LIB_DIR%\ftd2xx.lib" if exist "%RELEASE_LIB_DIR%\ftd2xx.lib" (
    echo ftd2xx.dll and ftd2xx.lib already exist in all target directories. Skipping copy.
    goto :EOF
)

REM Create necessary directories
mkdir "%DEBUG_BIN_DIR%" 2>nul
mkdir "%RELEASE_BIN_DIR%" 2>nul
mkdir "%DEBUG_LIB_DIR%" 2>nul
mkdir "%RELEASE_LIB_DIR%" 2>nul
mkdir "%TEMP_DIR%" 2>nul

REM Download the ZIP file if not already present
if not exist "%ZIP_FILE%" (
    echo Downloading FTDI D2XX driver...
    powershell -Command "Invoke-WebRequest -Uri '%FTDI_URL%' -OutFile '%ZIP_FILE%'"
)

REM Extract ZIP using PowerShell
echo Extracting FTDI driver...
powershell -Command "Expand-Archive -Path '%ZIP_FILE%' -DestinationPath '%TEMP_DIR%' -Force"

REM Copy DLLs to bin directories after renaming
echo Copying DLLs to bin directories...
set "DLL_FOUND=false"
for /R "%TEMP_DIR%" %%f in (ftd2xx64.dll) do (
    set "DLL_FOUND=true"
    if not exist "%DEBUG_BIN_DIR%\ftd2xx.dll" copy /Y "%%f" "%DEBUG_BIN_DIR%\ftd2xx.dll"
    if not exist "%RELEASE_BIN_DIR%\ftd2xx.dll" copy /Y "%%f" "%RELEASE_BIN_DIR%\ftd2xx.dll"
)

if "%DLL_FOUND%"=="false" (
    echo Warning: ftd2xx64.dll not found in extracted files.
)

REM Wait briefly before cleanup to avoid file lock issues
timeout /t 2 >nul

REM Copy LIBs
echo Copying LIBs to lib directories...
for /R "%TEMP_DIR%" %%f in (ftd2xx.lib) do (
    if not exist "%DEBUG_LIB_DIR%\ftd2xx.lib" copy /Y "%%f" "%DEBUG_LIB_DIR%"
    if not exist "%RELEASE_LIB_DIR%\ftd2xx.lib" copy /Y "%%f" "%RELEASE_LIB_DIR%"
)

REM Clean up
rmdir /S /Q "%TEMP_DIR%"

echo FTDI D2XX driver setup complete.

REM Pop back to original directory if we changed it
popd 2>nul

endlocal