$ErrorActionPreference = 'Stop'
$root = 'C:\ProdTools\qtac-refactor'
$build = "$root\build\Release"
$vcvars = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat'
$qt = 'C:\Qt\6.11.1\msvc2022_64'
$out = "$root\build_out.txt"

# Run configure + build inside a vcvars cmd shell, capturing output to file
$script = @"
call "$vcvars" >nul 2>&1
cmake -S "$root" -B "$build" -DCMAKE_PREFIX_PATH="$qt" -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=cl > "$out" 2>&1
if errorlevel 1 goto fail
cmake --build "$build" --target qtac-app TACDev test_tacdev_api test_hardware_psoc >> "$out" 2>&1
if errorlevel 1 goto fail
echo BUILD SUCCESS >> "$out"
exit /b 0
:fail
echo BUILD FAILED >> "$out"
exit /b 1
"@

$tmpBat = [System.IO.Path]::GetTempFileName() + '.bat'
[System.IO.File]::WriteAllText($tmpBat, $script)

Write-Host "Running build..."
$proc = Start-Process -FilePath 'cmd.exe' -ArgumentList "/c `"$tmpBat`"" -Wait -PassThru -WindowStyle Hidden
$exitCode = $proc.ExitCode

Get-Content $out
Write-Host "Exit code: $exitCode"
exit $exitCode
