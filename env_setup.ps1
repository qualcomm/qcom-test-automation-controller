# env_setup.ps1 — dot-source this to load MSVC + Qt into the current session.
# Usage: . "$PSScriptRoot\env_setup.ps1"

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
if (-not $vcvars -or -not (Test-Path $vcvars)) { throw "vcvars64.bat not found - is VS2022 installed?" }

# Locate Qt — honor QTDIR env var, else search standard install locations
if ($env:QTDIR -and (Test-Path "$env:QTDIR\bin\qmake.exe")) {
    $qtRoot = $env:QTDIR
} else {
    $qtRoot = Get-ChildItem 'C:\Qt' -Filter 'msvc2022_64' -Recurse -Depth 2 -Directory -ErrorAction SilentlyContinue |
              Where-Object { Test-Path "$($_.FullName)\bin\qmake.exe" } |
              Sort-Object FullName -Descending |
              Select-Object -First 1 -ExpandProperty FullName
}
if (-not $qtRoot) { throw "Qt not found. Set QTDIR or install Qt to C:\Qt\" }

# Import MSVC environment
$envLines = cmd /c "`"$vcvars`" && set" 2>&1
$envLines | Where-Object { $_ -match '^[A-Za-z_][A-Za-z0-9_]+=.' } | ForEach-Object {
    $kv = $_ -split '=', 2
    if ($kv.Count -eq 2) { [System.Environment]::SetEnvironmentVariable($kv[0], $kv[1]) }
}
$env:PATH = "$qtRoot\bin;$env:PATH"

Write-Host "MSVC env loaded from: $vcvars"
Write-Host "Qt root: $qtRoot"
