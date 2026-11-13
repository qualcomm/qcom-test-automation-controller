$ErrorActionPreference = 'Stop'
. "$PSScriptRoot\env_setup.ps1"

$logfile = "$PSScriptRoot\build_log.txt"

Set-Location $PSScriptRoot
if (Test-Path build) { Remove-Item -Recurse -Force build }

Write-Host '=== CMAKE CONFIGURE ==='
& cmake -S "$PSScriptRoot\source" -B build "-DCMAKE_PREFIX_PATH=$qtRoot" -DCMAKE_COLOR_DIAGNOSTICS=OFF -G 'Visual Studio 17 2022' -A x64 2>&1 | Tee-Object -FilePath $logfile
if ($LASTEXITCODE -ne 0) { Write-Host 'CONFIGURE FAILED'; exit 1 }

Write-Host '=== BUILD DEBUG ==='
& cmake --build build --config Debug 2>&1 | Tee-Object -FilePath $logfile -Append
if ($LASTEXITCODE -ne 0) { Write-Host 'DEBUG BUILD FAILED'; exit 1 }

Write-Host '=== BUILD RELEASE ==='
& cmake --build build --config Release 2>&1 | Tee-Object -FilePath $logfile -Append
if ($LASTEXITCODE -ne 0) { Write-Host 'RELEASE BUILD FAILED'; exit 1 }

Write-Host 'BUILD SUCCESS'
exit 0
