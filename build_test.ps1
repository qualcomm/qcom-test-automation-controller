$ErrorActionPreference = 'Continue'
$vcvars = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat'
$root = 'C:\ProdTools\qtac-refactor'

# Import MSVC environment
$envLines = cmd /c "`"$vcvars`" && set" 2>&1
$envLines | Where-Object { $_ -match '^[A-Za-z_][A-Za-z0-9_]+=.' } | ForEach-Object {
    $kv = $_ -split '=', 2
    if ($kv.Count -eq 2) {
        [System.Environment]::SetEnvironmentVariable($kv[0], $kv[1])
    }
}
Write-Output "MSVC env loaded"

Set-Location $root

$buildDir = "build_test\Debug"

# Configure
Write-Output "--- Configuring tests ---"
& cmake -S "source\test" -B $buildDir -G Ninja -DCMAKE_BUILD_TYPE=Debug
if ($LASTEXITCODE -ne 0) { Write-Error "Configure failed"; exit 1 }

# Build only test_hardware_psoc
$target = if ($args.Count -gt 0) { $args[0] } else { "test_hardware_psoc" }
Write-Output "--- Building $target ---"
& cmake --build $buildDir --target $target
if ($LASTEXITCODE -ne 0) { Write-Error "Build failed"; exit 1 }

Write-Output "--- BUILD SUCCESS ---"
Write-Output "Binary: $root\$buildDir\$target.exe"
