$ErrorActionPreference = 'Continue'
$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (-not (Test-Path $vswhere)) { Write-Error "vswhere.exe not found — is Visual Studio installed?"; exit 1 }
$vsInstallPath = & $vswhere -latest -products * -requires Microsoft.VisualCpp.Tools.HostX64.TargetX64 -property installationPath
if (-not $vsInstallPath) { Write-Error "No VS install with VC++ x64 tools found"; exit 1 }
$vcvars = "$vsInstallPath\VC\Auxiliary\Build\vcvars64.bat"
$qtbin = 'C:\Qt\6.11.1\msvc2022_64\bin'
$root = 'C:\ProdTools\qtac-refactor'

# Import MSVC environment
$envLines = cmd /c "`"$vcvars`" && set" 2>&1
$envLines | Where-Object { $_ -match '^[A-Za-z_][A-Za-z0-9_]+=.' } | ForEach-Object {
    $kv = $_ -split '=', 2
    if ($kv.Count -eq 2) {
        [System.Environment]::SetEnvironmentVariable($kv[0], $kv[1])
    }
}
$env:PATH = "$qtbin;$env:PATH"
Write-Output "MSVC env loaded"

$clPath = (Get-Command cl.exe -ErrorAction SilentlyContinue).Source
Write-Output "cl.exe: $clPath"

Set-Location $root

# Configure
Write-Output "--- Configuring ---"
& cmake -S . -B "build\Release" "-DCMAKE_PREFIX_PATH=$qtbin\.." -G Ninja -DCMAKE_BUILD_TYPE=Release
if ($LASTEXITCODE -ne 0) { Write-Error "Configure failed"; exit 1 }

# Build qtac-app and TACDev
Write-Output "--- Building qtac-app ---"
& cmake --build "build\Release" --target qtac-app
if ($LASTEXITCODE -ne 0) { Write-Error "Build failed"; exit 1 }

Write-Output "--- Building TACDev ---"
& cmake --build "build\Release" --target TACDev
if ($LASTEXITCODE -ne 0) { Write-Error "Build failed"; exit 1 }

Write-Output "--- BUILD SUCCESS ---"
