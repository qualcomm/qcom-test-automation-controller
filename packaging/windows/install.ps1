# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

[CmdletBinding()]
param(
    [string] $InstallRoot = "$env:ProgramFiles\Qualcomm\QTAC",
    [string] $DataRoot    = "$env:ProgramData\Qualcomm\QTAC",
    [switch] $NoElevate,   # skip UAC self-elevation (testing)
    [switch] $SkipDrivers  # skip FTDICheck driver install (testing)
)

$ErrorActionPreference = 'Stop'
$root = $PSScriptRoot
$appName   = 'QTAC'
$appDisplay = 'Qualcomm Test Automation Controller'
$publisher = 'Qualcomm Technologies, Inc.'
$version = (Get-Content (Join-Path $root 'version.txt') -ErrorAction SilentlyContinue | Select-Object -First 1)
if (-not $version) { $version = '0.0.0' }

$ExamplesRoot = 'C:\QTAC\examples'

# Install log 
$logFile = Join-Path $env:TEMP 'QTAC-install.log'
function Log($m) { try { Add-Content -LiteralPath $logFile -Value ("{0} [{1}] {2}" -f (Get-Date -Format 'HH:mm:ss'), $PID, $m) } catch {} }
trap { Log "FATAL: $($_.Exception.Message)"; throw }
Log "start root=[$root] NoElevate=$NoElevate SkipDrivers=$SkipDrivers cmd=[$($MyInvocation.Line)]"

function Test-Admin {
    $id = [Security.Principal.WindowsIdentity]::GetCurrent()
    (New-Object Security.Principal.WindowsPrincipal $id).IsInRole(
        [Security.Principal.WindowsBuiltinRole]::Administrator)
}

function Test-FtdiDriversPresent {
    $pnpOut = (& pnputil /enum-drivers 2>&1) | ForEach-Object { "$_" }
    [bool]($pnpOut | Select-String -Pattern 'ftdibus|ftdiport' -Quiet)
}

function Save-FtdiDriverManifest {
    param([string] $ManifestPath)
    $pnpOut = (& pnputil /enum-drivers 2>&1) | ForEach-Object { "$_" }
    $ftdiInfs   = @('ftdibus.inf', 'ftdiport.inf')
    $currentOem = $null
    $found      = @()
    foreach ($line in $pnpOut) {
        if ($line -match 'Published Name\s*:\s*(oem\d+\.inf)') {
            $currentOem = $Matches[1]
        } elseif ($line -match 'Original Name\s*:\s*(\S+\.inf)') {
            $origName = $Matches[1].Trim()
            if ($ftdiInfs -contains $origName -and $currentOem) {
                $found += $currentOem
                $currentOem = $null
            }
        }
    }
    if ($found.Count -gt 0) {
        $found | Set-Content -LiteralPath $ManifestPath
        Log "saved FTDI driver manifest ($($found -join ', ')) -> $ManifestPath"
    } else {
        Log "no FTDI oem.inf entries found to save into manifest"
    }
}

if (-not $NoElevate -and -not (Test-Admin)) {
    $argList = @('-NoProfile', '-ExecutionPolicy', 'Bypass', '-File', "`"$PSCommandPath`"",
                 '-InstallRoot', "`"$InstallRoot`"", '-DataRoot', "`"$DataRoot`"")
    if ($SkipDrivers) { $argList += '-SkipDrivers' }
    Start-Process -FilePath 'powershell.exe' -ArgumentList $argList -Verb RunAs -Wait
    exit $LASTEXITCODE
}

# If a previous QTAC install is present, remove it completely before installation
$arpKey = "HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$appName"
if ((Test-Path $arpKey) -or (Test-Path $InstallRoot)) {
    Write-Host "Existing $appDisplay installation detected - removing it first..."
    Log "existing install detected (ARP=$(Test-Path $arpKey) InstallRoot=$(Test-Path $InstallRoot)) - uninstalling before install"

    # Stop running app processes so their files aren't locked during removal/overwrite.
    $procNames = @('TAC', 'TACConfigEditor', 'DeviceCatalog', 'LITEProgrammer', 'FTDICheck')
    foreach ($p in $procNames) {
        Get-Process -Name $p -ErrorAction SilentlyContinue | ForEach-Object {
            Log "stopping running process $($_.Name) (pid $($_.Id))"
            Stop-Process -Id $_.Id -Force -ErrorAction SilentlyContinue
        }
    }
    Start-Sleep -Milliseconds 500

    $uninstallScript = Join-Path $root 'uninstall.ps1'
    if (Test-Path $uninstallScript) {
        & powershell.exe -NoProfile -ExecutionPolicy Bypass -File $uninstallScript `
            -InstallRoot $InstallRoot -DataRoot $DataRoot -NoElevate
        Log "previous-install uninstall.ps1 exit code: $LASTEXITCODE"
    } else {
        Log "WARNING: uninstall.ps1 not found at $uninstallScript - skipping pre-install cleanup"
    }

    if (Test-Path $InstallRoot) {
        Write-Warning "Some files under $InstallRoot could not be removed (likely still in use). Installation will continue and overwrite what it can."
        Log "InstallRoot still present after pre-install uninstall"
    } else {
        Log "previous install removed successfully"
    }
}

function Copy-Tree($from, $to) {
    if (-not (Test-Path $from)) { return }
    New-Item -ItemType Directory -Force -Path $to | Out-Null
    Copy-Item -Path (Join-Path $from '*') -Destination $to -Recurse -Force
}

$src = $root
$zip = Join-Path $root 'payload.zip'
if (Test-Path $zip) {
    $src = Join-Path $root '_payload'
    Remove-Item $src -Recurse -Force -ErrorAction SilentlyContinue
    Log "expanding payload.zip -> $src"
    Add-Type -AssemblyName System.IO.Compression.FileSystem
    [System.IO.Compression.ZipFile]::ExtractToDirectory($zip, $src)
}

Write-Host "Installing $appDisplay $version"
Write-Host "  Program files : $InstallRoot"
Write-Host "  Shared data   : $DataRoot"
Write-Host "  Examples      : $ExamplesRoot"

# 1. Application binaries + Qt runtime + docs
Copy-Tree (Join-Path $src 'app')  $InstallRoot
Copy-Tree (Join-Path $src 'docs') (Join-Path $InstallRoot 'docs')

# 2. Shared, machine-wide data
Copy-Tree (Join-Path $src 'configurations') (Join-Path $DataRoot 'configurations')
Copy-Tree (Join-Path $src 'FTDI')           (Join-Path $DataRoot 'FTDI')

# 2a. Interfaces (Python + Java SDKs only)
Copy-Tree (Join-Path $src 'interfaces') (Join-Path $DataRoot 'interfaces')
Log "interfaces copied to $(Join-Path $DataRoot 'interfaces')"

# 2b. Examples -> fixed top-level
Copy-Tree (Join-Path $src 'examples') $ExamplesRoot
Log "examples copied to $ExamplesRoot"

# 3. Bundle the uninstaller alongside the app
Copy-Item (Join-Path $root 'uninstall.ps1') $InstallRoot -Force
Copy-Item (Join-Path $root 'uninstall.exe') $InstallRoot -Force -ErrorAction SilentlyContinue
Copy-Item (Join-Path $root 'version.txt')   $InstallRoot -Force -ErrorAction SilentlyContinue

# 4. All-users Start Menu folder with shortcuts for each application
$startMenuFolder = Join-Path $env:ProgramData "Microsoft\Windows\Start Menu\Programs\$appDisplay"
New-Item -ItemType Directory -Force -Path $startMenuFolder | Out-Null
Log "start-menu folder: $startMenuFolder"

$wsh = New-Object -ComObject WScript.Shell
$shortcuts = @(
    [ordered]@{ Name = 'Test Automation Controller'; Exe = 'TAC.exe';             Desc = 'Test Automation Controller Application' },
    [ordered]@{ Name = 'TAC Configuration Editor';   Exe = 'TACConfigEditor.exe'; Desc = 'TAC Configuration Editor Application' },
    [ordered]@{ Name = 'Device Catalog';             Exe = 'DeviceCatalog.exe';   Desc = 'Device Catalog Application' },
    [ordered]@{ Name = 'LITE Programmer';            Exe = 'LITEProgrammer.exe';  Desc = 'LITE Programmer Application' },
    [ordered]@{ Name = 'Uninstall QTAC';             Exe = 'uninstall.exe';       Desc = 'QTAC Uninstall' }
)
foreach ($s in $shortcuts) {
    $exePath = Join-Path $InstallRoot $s.Exe
    if (Test-Path $exePath) {
        $lnk = Join-Path $startMenuFolder "$($s.Name).lnk"
        $sc = $wsh.CreateShortcut($lnk)
        $sc.TargetPath       = $exePath
        $sc.WorkingDirectory = $InstallRoot
        $sc.IconLocation     = "$exePath,0"
        $sc.Description      = $s.Desc
        $sc.Save()
        Log "shortcut created: $lnk"
    } else {
        Log "skipped shortcut (not found): $exePath"
    }
}

# 5. Register .tcnf file association
$tacConfigEditorExe = Join-Path $InstallRoot 'TACConfigEditor.exe'
if (Test-Path $tacConfigEditorExe) {
    $tcnfProgId = 'QTAC.TACConfig'

    New-Item -Path 'HKLM:\SOFTWARE\Classes\.tcnf' -Force | Out-Null
    Set-ItemProperty 'HKLM:\SOFTWARE\Classes\.tcnf' '(Default)' $tcnfProgId

    New-Item -Path "HKLM:\SOFTWARE\Classes\$tcnfProgId" -Force | Out-Null
    Set-ItemProperty "HKLM:\SOFTWARE\Classes\$tcnfProgId" '(Default)' 'QTAC Configuration File'

    New-Item -Path "HKLM:\SOFTWARE\Classes\$tcnfProgId\DefaultIcon" -Force | Out-Null
    Set-ItemProperty "HKLM:\SOFTWARE\Classes\$tcnfProgId\DefaultIcon" '(Default)' "`"$tacConfigEditorExe`",0"

    New-Item -Path "HKLM:\SOFTWARE\Classes\$tcnfProgId\shell\open\command" -Force | Out-Null
    Set-ItemProperty "HKLM:\SOFTWARE\Classes\$tcnfProgId\shell\open\command" '(Default)' "`"$tacConfigEditorExe`" `"%1`""

    try {
        Add-Type -Namespace QTAC -Name Shell32 -MemberDefinition @'
[System.Runtime.InteropServices.DllImport("shell32.dll")]
public static extern void SHChangeNotify(int wEventId, int uFlags, IntPtr dwItem1, IntPtr dwItem2);
'@ -ErrorAction Stop
        [QTAC.Shell32]::SHChangeNotify(0x08000000, 0x0000, [IntPtr]::Zero, [IntPtr]::Zero)  # SHCNE_ASSOCCHANGED
        Log ".tcnf association change notified to Explorer (SHChangeNotify)"
    } catch {
        Log "WARNING: SHChangeNotify failed (non-fatal, association still registered): $($_.Exception.Message)"
    }

    Log ".tcnf file association registered -> $tacConfigEditorExe"
} else {
    Log "skipped .tcnf file association (TACConfigEditor.exe not found): $tacConfigEditorExe"
}

# 6. Add/Remove Programs + winget tracking
$tacExe       = Join-Path $InstallRoot 'TAC.exe'
$uninstallExe = Join-Path $InstallRoot 'uninstall.exe'
$uninstallCmd = if (Test-Path $uninstallExe) { "`"$uninstallExe`"" } `
                else { "powershell.exe -NoProfile -ExecutionPolicy Bypass -File `"$InstallRoot\uninstall.ps1`"" }
$key = "HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$appName"
New-Item -Path $key -Force | Out-Null
Set-ItemProperty $key DisplayName     $appDisplay
Set-ItemProperty $key DisplayVersion  $version
Set-ItemProperty $key Publisher       $publisher
Set-ItemProperty $key InstallLocation $InstallRoot
Set-ItemProperty $key DisplayIcon     $tacExe
Set-ItemProperty $key UninstallString $uninstallCmd
Set-ItemProperty $key NoModify 1 -Type DWord
Set-ItemProperty $key NoRepair 1 -Type DWord

# 7. FTDI driver 
if (-not $SkipDrivers) {
    $ftdiCheck = Join-Path $InstallRoot 'FTDICheck.exe'
    if (Test-Path $ftdiCheck) {
        Write-Host 'Installing/verifying FTDI driver...'
        Log "running FTDICheck: $ftdiCheck"
        & $ftdiCheck
        $ftdiExitCode = $LASTEXITCODE
        Log "FTDICheck exit code: $ftdiExitCode"
        if ($ftdiExitCode -ne 0) {
            Write-Warning "FTDICheck exited with code $ftdiExitCode - falling back to direct pnputil install"
            Log "FTDICheck failed - attempting direct pnputil fallback"
        }
    } else {
        Write-Warning 'FTDICheck.exe not found; skipping FTDI driver step.'
        Log "FTDICheck.exe not found at $ftdiCheck"
    }

    if (-not (Test-FtdiDriversPresent)) {
        Write-Host 'FTDI drivers not found in driver store - installing via pnputil directly...'
        Log "FTDI drivers absent - running pnputil fallback"
        $ftdiInfDir = Join-Path $DataRoot 'FTDI'
        $infPaths = @(
            (Join-Path $ftdiInfDir 'arm\ARM64\Release\FTDIBUS.inf'),
            (Join-Path $ftdiInfDir 'arm\ARM64\Release\FTDIPORT.inf'),
            (Join-Path $ftdiInfDir 'x64\ftdibus.inf'),
            (Join-Path $ftdiInfDir 'x64\ftdiport.inf')
        )
        foreach ($inf in $infPaths) {
            if (Test-Path $inf) {
                Write-Host "  Installing $inf ..."
                (& pnputil /add-driver $inf /install 2>&1) | ForEach-Object { Log "  pnputil: $_" }
                Log "  pnputil exit: $LASTEXITCODE"
            }
        }
        if (Test-FtdiDriversPresent) {
            Write-Host 'FTDI drivers installed successfully via pnputil.'
            Log "FTDI drivers installed via pnputil fallback"
        } else {
            Write-Warning 'FTDI driver installation failed. Run FTDICheck.exe manually from an elevated prompt.'
            Log "FTDI driver installation failed"
        }
    } else {
        Log "FTDI drivers confirmed present in driver store"
    }

    Save-FtdiDriverManifest -ManifestPath (Join-Path $InstallRoot 'ftdi-drivers.txt')
}

Write-Host "$appDisplay $version installed."