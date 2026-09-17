$ErrorActionPreference = 'Continue'
$root = $PSScriptRoot

# Locate VS via vswhere
$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (Test-Path $vswhere) {
    $vsInstallPath = & $vswhere -latest -products * -requires Microsoft.VisualCpp.Tools.HostX64.TargetX64 -property installationPath
    $vcvars = "$vsInstallPath\VC\Auxiliary\Build\vcvars64.bat"
} else {
    $vcvars = @(
        "${env:ProgramFiles}\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat",
        "${env:ProgramFiles}\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat",
        "${env:ProgramFiles}\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat",
        "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
    ) | Where-Object { Test-Path $_ } | Select-Object -First 1
}
if (-not $vcvars -or -not (Test-Path $vcvars)) { Write-Error "vcvars64.bat not found - is VS2022 installed?"; exit 1 }

# Locate Qt — honor QTDIR env var, else search standard install locations
if ($env:QTDIR -and (Test-Path "$env:QTDIR\bin\qmake.exe")) {
    $qtRoot = $env:QTDIR
} else {
    $qtRoot = Get-ChildItem 'C:\Qt' -Filter 'msvc2022_64' -Recurse -Depth 2 -Directory -ErrorAction SilentlyContinue |
              Where-Object { Test-Path "$($_.FullName)\bin\qmake.exe" } |
              Sort-Object FullName -Descending |
              Select-Object -First 1 -ExpandProperty FullName
}
if (-not $qtRoot) { Write-Error "Qt not found. Set QTDIR or install Qt to C:\Qt\"; exit 1 }
$qtbin = "$qtRoot\bin"

# Import MSVC environment
$envLines = cmd /c "`"$vcvars`" && set" 2>&1
$envLines | Where-Object { $_ -match '^[A-Za-z_][A-Za-z0-9_]+=.' } | ForEach-Object {
    $kv = $_ -split '=', 2
    if ($kv.Count -eq 2) { [System.Environment]::SetEnvironmentVariable($kv[0], $kv[1]) }
}
$env:PATH = "$qtbin;$env:PATH"
Write-Output "MSVC env loaded from: $vcvars"
Write-Output "Qt root: $qtRoot"
Write-Output "cl.exe: $((Get-Command cl.exe -ErrorAction SilentlyContinue).Source)"

Set-Location $root

# Configure
Write-Output "--- Configuring ---"
& cmake -S "source" -B "build\Release" "-DCMAKE_PREFIX_PATH=$qtRoot" -G Ninja -DCMAKE_BUILD_TYPE=Release
if ($LASTEXITCODE -ne 0) { Write-Error "Configure failed"; exit 1 }

# Build qtac-app and TACDev
Write-Output "--- Building qtac-app ---"
& cmake --build "build\Release" --target qtac-app
if ($LASTEXITCODE -ne 0) { Write-Error "Build failed"; exit 1 }

Write-Output "--- Building TACDev ---"
& cmake --build "build\Release" --target TACDev
if ($LASTEXITCODE -ne 0) { Write-Error "Build failed"; exit 1 }

Write-Output "--- BUILD SUCCESS ---"
