[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [ValidatePattern("^[0-9A-Za-z][0-9A-Za-z._-]*$")]
    [string]$Version,

    [string]$QtRoot = $env:QTAC_QT_ROOT,

    [string]$CMakePath = $env:CMAKE_EXE_PATH,

    [string]$PolicyPath,

    [string]$BuildRoot,

    [string]$OutputRoot,

    [switch]$SkipBuild,

    [switch]$AllowDirty
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$repositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path

if ([string]::IsNullOrWhiteSpace($PolicyPath)) {
    $PolicyPath = Join-Path $repositoryRoot "release-policy.json"
}

if ([string]::IsNullOrWhiteSpace($BuildRoot)) {
    $BuildRoot = Join-Path $repositoryRoot "build-release-msvc"
}

if ([string]::IsNullOrWhiteSpace($OutputRoot)) {
    $OutputRoot = Join-Path $repositoryRoot "artifacts"
}

function Resolve-SafeChildPath {
    param(
        [string]$Parent,
        [string]$Child,
        [string]$Description
    )

    $parentPath = [System.IO.Path]::GetFullPath($Parent).TrimEnd('\', '/')
    $childPath = [System.IO.Path]::GetFullPath($Child).TrimEnd('\', '/')
    $rootPath = [System.IO.Path]::GetPathRoot($childPath).TrimEnd('\', '/')

    if ($childPath -eq $rootPath -or
        -not $childPath.StartsWith(
            $parentPath + [System.IO.Path]::DirectorySeparatorChar,
            [StringComparison]::OrdinalIgnoreCase)) {
        throw "$Description must be a non-root child of $parentPath; received $childPath."
    }

    return $childPath
}

function Assert-NoReparsePointPath {
    param(
        [string]$Parent,
        [string]$Child,
        [string]$Description
    )

    $parentPath = [System.IO.Path]::GetFullPath($Parent).TrimEnd('\', '/')
    $currentPath = [System.IO.Path]::GetFullPath($Child).TrimEnd('\', '/')

    while ($currentPath -ne $parentPath) {
        if (Test-Path -LiteralPath $currentPath) {
            $item = Get-Item -LiteralPath $currentPath -Force
            if (($item.Attributes -band [System.IO.FileAttributes]::ReparsePoint) -ne 0) {
                throw "$Description must not contain reparse points: $currentPath"
            }
        }

        $nextPath = Split-Path -Parent $currentPath
        if ([string]::IsNullOrWhiteSpace($nextPath) -or $nextPath -eq $currentPath) {
            throw "$Description escapes its expected parent: $Child"
        }

        $currentPath = $nextPath.TrimEnd('\', '/')
    }
}

function Resolve-Tool {
    param(
        [string]$RequestedPath,
        [string]$CommandName,
        [string[]]$FallbackPaths
    )

    if (-not [string]::IsNullOrWhiteSpace($RequestedPath)) {
        if (-not (Test-Path -Path $RequestedPath -PathType Leaf)) {
            throw "$CommandName path does not exist: $RequestedPath"
        }

        return (Resolve-Path $RequestedPath).Path
    }

    $command = Get-Command $CommandName -ErrorAction SilentlyContinue
    if ($null -ne $command) {
        return $command.Source
    }

    foreach ($fallback in $FallbackPaths) {
        if (Test-Path -Path $fallback -PathType Leaf) {
            return (Resolve-Path $fallback).Path
        }
    }

    throw "$CommandName was not found. Pass its path or configure the corresponding environment variable."
}

function Invoke-Process {
    param(
        [string]$FilePath,
        [string[]]$Arguments,
        [string]$Description
    )

    & $FilePath @Arguments
    $exitCode = $LASTEXITCODE

    if ($exitCode -ne 0) {
        throw "$Description failed with exit code $exitCode."
    }
}

function Copy-MatchingFiles {
    param(
        [string]$Source,
        [string]$Destination,
        [string]$Pattern
    )

    if (-not (Test-Path -LiteralPath $Source -PathType Container)) {
        throw "Required release directory is missing: $Source"
    }

    foreach ($file in @(Get-ChildItem -LiteralPath $Source -Filter $Pattern -File -Recurse)) {
        $relativePath = $file.FullName.Substring($Source.Length).TrimStart('\', '/')
        $targetPath = Join-Path $Destination $relativePath
        New-Item -ItemType Directory -Path (Split-Path -Parent $targetPath) -Force | Out-Null
        Copy-Item -LiteralPath $file.FullName -Destination $targetPath -Force
    }
}

function Copy-DirectoryIfPresent {
    param(
        [string]$Source,
        [string]$Destination,
        [bool]$Required = $false
    )

    if (Test-Path -Path $Source -PathType Container) {
        New-Item -ItemType Directory -Path $Destination -Force | Out-Null
        Copy-Item -Path (Join-Path $Source "*") -Destination $Destination -Recurse -Force
    }
    elseif ($Required) {
        throw "Required release directory is missing: $Source"
    }
}

$policyPath = (Resolve-Path $PolicyPath).Path
$policy = Get-Content -Path $policyPath -Raw | ConvertFrom-Json

if ($policy.product -ne "QTAC" -or
    $policy.configuration -ne "Release" -or
    $policy.architecture -ne "x64" -or
    $policy.runtimeOutput -ne "__Builds/x64/Release/bin/Release" -or
    $policy.configurationPath -ne "configurations" -or
    $policy.configurationPattern -ne "*.tcnf") {
    throw "release-policy.json contains unsupported product, build, runtime, or configuration paths."
}

if (-not $AllowDirty) {
    $dirty = @(& git -C $repositoryRoot status --porcelain)
    if ($dirty.Count -gt 0) {
        throw "Refusing to build a release from a dirty source tree. Commit/stash changes or use -AllowDirty for local validation only."
    }
}

$sourceCommit = (& git -C $repositoryRoot rev-parse HEAD).Trim()
if ([string]::IsNullOrWhiteSpace($sourceCommit)) {
    throw "Unable to resolve the source commit."
}

if (-not $SkipBuild) {
    if ([string]::IsNullOrWhiteSpace($CMakePath)) {
        $CMakePath = Resolve-Tool `
            -RequestedPath $null `
            -CommandName "cmake.exe" `
            -FallbackPaths @("C:\Program Files\CMake\bin\cmake.exe")
    }
    else {
        $CMakePath = Resolve-Tool `
            -RequestedPath $CMakePath `
            -CommandName "cmake.exe" `
            -FallbackPaths @()
    }

    if ([string]::IsNullOrWhiteSpace($QtRoot)) {
        throw "Set QTAC_QT_ROOT or pass -QtRoot, for example C:\Qt\6.9.3\msvc2022_64."
    }

    $QtRoot = (Resolve-Path $QtRoot).Path
    if (-not (Test-Path -Path (Join-Path $QtRoot "bin\windeployqt.exe") -PathType Leaf)) {
        throw "QtRoot does not contain bin\windeployqt.exe: $QtRoot"
    }
}

$BuildRoot = Resolve-SafeChildPath -Parent $repositoryRoot -Child $BuildRoot -Description "BuildRoot"
if ((Split-Path -Parent $BuildRoot) -ne $repositoryRoot -or
    -not (Split-Path -Leaf $BuildRoot).StartsWith("build-", [StringComparison]::OrdinalIgnoreCase)) {
    throw "BuildRoot must be a direct repository child whose leaf starts with 'build-': $BuildRoot"
}

$OutputRoot = [System.IO.Path]::GetFullPath($OutputRoot)
$architecture = [string]$policy.architecture
$configuration = [string]$policy.configuration
$artifactName = "QTAC-$Version-windows-$architecture"
$artifactRoot = Resolve-SafeChildPath -Parent $OutputRoot -Child (Join-Path $OutputRoot $artifactName) -Description "Artifact staging root"
$zipPath = Join-Path $OutputRoot "$artifactName.zip"
Assert-NoReparsePointPath -Parent $OutputRoot -Child $artifactRoot -Description "Artifact staging root"
$runtimeSource = Resolve-SafeChildPath -Parent $repositoryRoot -Child (Join-Path $repositoryRoot ([string]$policy.runtimeOutput)) -Description "Runtime output"
$configSource = Resolve-SafeChildPath -Parent $repositoryRoot -Child (Join-Path $repositoryRoot ([string]$policy.configurationPath)) -Description "Configuration source"
Assert-NoReparsePointPath -Parent $repositoryRoot -Child $BuildRoot -Description "BuildRoot"
Assert-NoReparsePointPath -Parent $repositoryRoot -Child $runtimeSource -Description "Runtime output"
Assert-NoReparsePointPath -Parent $repositoryRoot -Child $configSource -Description "Configuration source"
$examplesSource = Join-Path $repositoryRoot "examples"
$pythonSource = Join-Path $repositoryRoot "interfaces\Python"
$javaSource = Join-Path $repositoryRoot "interfaces\Java"
$docsSource = Join-Path $repositoryRoot "docs"
$releaseNotesSource = Join-Path $repositoryRoot "CHANGELOG.md"

if (-not $SkipBuild) {
    if (Test-Path $BuildRoot) {
        Remove-Item -Path $BuildRoot -Recurse -Force
    }

    if (Test-Path $runtimeSource) {
        Remove-Item -Path $runtimeSource -Recurse -Force
    }

    Invoke-Process `
        -FilePath $CMakePath `
        -Arguments @(
            "-S", $repositoryRoot,
            "-B", $BuildRoot,
            "-G", "Visual Studio 17 2022",
            "-A", "x64",
            "-DCMAKE_PREFIX_PATH=$QtRoot"
        ) `
        -Description "QTAC CMake configure"

    Invoke-Process `
        -FilePath $CMakePath `
        -Arguments @(
            "--build", $BuildRoot,
            "--config", "Release",
            "--parallel", "4"
        ) `
        -Description "QTAC Release build"
}

if (-not (Test-Path -Path $runtimeSource -PathType Container)) {
    throw "QTAC Release runtime output is missing: $runtimeSource. The project controls this output through Common.cmake."
}

$requiredFiles = @($policy.requiredExecutables) + @($policy.requiredInterfaces)
$missing = @(
    $requiredFiles | Where-Object {
        -not (Test-Path -Path (Join-Path $runtimeSource ([string]$_)) -PathType Leaf)
    }
)

if ($missing.Count -gt 0) {
    throw "QTAC Release output is incomplete. Missing: $($missing -join ', ')"
}

$configurations = @(
    Get-ChildItem `
        -Path $configSource `
        -Filter ([string]$policy.configurationPattern) `
        -File `
        -Recurse
)

$expectedConfigurationCount = [int]$policy.expectedConfigurationCount
if ($configurations.Count -ne $expectedConfigurationCount) {
    throw "QTAC configuration count mismatch. Expected $expectedConfigurationCount, found $($configurations.Count)."
}

if (Test-Path $artifactRoot) {
    Remove-Item -Path $artifactRoot -Recurse -Force
}

New-Item -ItemType Directory -Path $artifactRoot -Force | Out-Null
Copy-DirectoryIfPresent -Source $runtimeSource -Destination (Join-Path $artifactRoot "bin") -Required $true
Copy-MatchingFiles -Source $configSource -Destination (Join-Path $artifactRoot "configurations") -Pattern ([string]$policy.configurationPattern)
Copy-DirectoryIfPresent -Source $examplesSource -Destination (Join-Path $artifactRoot "examples")
Copy-DirectoryIfPresent -Source $pythonSource -Destination (Join-Path $artifactRoot "python")
Copy-DirectoryIfPresent -Source $javaSource -Destination (Join-Path $artifactRoot "java")

$documentationRequired = [bool]$policy.documentationRequired
Copy-DirectoryIfPresent `
    -Source $docsSource `
    -Destination (Join-Path $artifactRoot "docs") `
    -Required $documentationRequired

if (Test-Path -Path $releaseNotesSource -PathType Leaf) {
    Copy-Item -Path $releaseNotesSource -Destination (Join-Path $artifactRoot "Release Notes.txt") -Force
}

$forbiddenExtensions = @($policy.forbiddenExtensions)
$forbiddenFiles = @($policy.forbiddenFiles)
$forbidden = @(
    Get-ChildItem -Path $artifactRoot -File -Recurse | Where-Object {
        $_.Extension -in $forbiddenExtensions -or
        $_.Name -in $forbiddenFiles
    }
)

if ($forbidden.Count -gt 0) {
    $forbiddenPaths = $forbidden | ForEach-Object {
        $_.FullName.Substring($artifactRoot.Length).TrimStart('\', '/')
    }

    throw "QTAC public release contains restricted or unsupported payload: $($forbiddenPaths -join ', ')"
}

$installerScript = @'
param(
    [string]$InstallRoot = "${env:ProgramFiles}\Qualcomm\QTAC",
    [string]$DataRoot = "${env:ProgramData}\Qualcomm\Alpaca\tac_configs"
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$packageRoot = Split-Path -Parent $PSCommandPath
New-Item -ItemType Directory -Path $InstallRoot -Force | Out-Null
New-Item -ItemType Directory -Path $DataRoot -Force | Out-Null

Copy-Item -Path (Join-Path $packageRoot "bin\*") -Destination $InstallRoot -Recurse -Force
Copy-Item -Path (Join-Path $packageRoot "configurations\*") -Destination $DataRoot -Recurse -Force

Write-Output "QTAC installed to: $InstallRoot"
Write-Output "TAC configurations installed to: $DataRoot"
'@

Set-Content -Path (Join-Path $artifactRoot "Install-QTAC.ps1") -Value $installerScript -Encoding utf8

$files = @(
    Get-ChildItem -Path $artifactRoot -File -Recurse | ForEach-Object {
        [PSCustomObject]@{
            path = $_.FullName.Substring($artifactRoot.Length).TrimStart('\', '/').Replace('\', '/')
            sha256 = (Get-FileHash -Path $_.FullName -Algorithm SHA256).Hash
            size = $_.Length
        }
    }
)

$cmakeVersion = if ($SkipBuild) {
    $null
}
else {
    (& $CMakePath --version | Select-Object -First 1)
}

$manifest = [PSCustomObject]@{
    schemaVersion = 1
    product = "QTAC"
    version = $Version
    configuration = $configuration
    architecture = $architecture
    sourceCommit = $sourceCommit
    sourceCommitVerifiedByBuild = (-not [bool]$SkipBuild -and -not [bool]$AllowDirty)
    buildPerformed = -not [bool]$SkipBuild
    sourceTreeDirtyOverride = [bool]$AllowDirty
    generatedUtc = (Get-Date).ToUniversalTime().ToString("o")
    policyFile = Split-Path -Leaf $policyPath
    runtimeOutput = [string]$policy.runtimeOutput
    expectedConfigurationCount = $expectedConfigurationCount
    configurationCount = $configurations.Count
    documentationIncluded = (Test-Path -Path (Join-Path $artifactRoot "docs") -PathType Container)
    cmakeVersion = $cmakeVersion
    forbiddenExtensions = $forbiddenExtensions
    forbiddenFiles = $forbiddenFiles
    files = $files
}

$manifest |
    ConvertTo-Json -Depth 8 |
    Set-Content -Path (Join-Path $artifactRoot "release-manifest.json") -Encoding utf8

New-Item -ItemType Directory -Path $OutputRoot -Force | Out-Null
if (Test-Path $zipPath) {
    Remove-Item -Path $zipPath -Force
}

Compress-Archive -Path (Join-Path $artifactRoot "*") -DestinationPath $zipPath -CompressionLevel Optimal

Write-Output "QTAC standalone release created:"
Write-Output "  Staging: $artifactRoot"
Write-Output "  Archive: $zipPath"
Write-Output "  Archive SHA-256: $((Get-FileHash -Path $zipPath -Algorithm SHA256).Hash)"