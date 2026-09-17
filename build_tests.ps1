$ErrorActionPreference = 'Stop'
. "$PSScriptRoot\env_setup.ps1"

$build = "$PSScriptRoot\build\Release"
$out   = "$PSScriptRoot\build_out.txt"

Write-Host "--- Configuring ---"
& cmake -S "$PSScriptRoot\source" -B "$build" "-DCMAKE_PREFIX_PATH=$qtRoot" -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=cl 2>&1 | Tee-Object -FilePath $out
if ($LASTEXITCODE -ne 0) { Write-Error "Configure failed"; exit 1 }

Write-Host "--- Building ---"
& cmake --build "$build" --target qtac-app TACDev test_tacdev_api test_hardware_psoc 2>&1 | Tee-Object -FilePath $out -Append
if ($LASTEXITCODE -ne 0) { Write-Error "Build failed"; exit 1 }

Write-Host "BUILD SUCCESS"
