# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

[CmdletBinding()]
param(
    [string] $InstallRoot  = "$env:ProgramFiles\Qualcomm\QTAC",
    [string] $DataRoot     = "$env:ProgramData\Qualcomm\QTAC",
    [switch] $NoElevate,
    [switch] $KeepData,    
    [switch] $KeepDrivers  
)

$ErrorActionPreference = 'Stop'
$appName    = 'QTAC'
$appDisplay = 'Qualcomm Test Automation Controller'

$logFile = Join-Path $env:TEMP 'QTAC-uninstall.log'
function Log($m) { try { Add-Content -LiteralPath $logFile -Value ("{0} [{1}] {2}" -f (Get-Date -Format 'HH:mm:ss'), $PID, $m) } catch {} }
trap { Log "FATAL: $($_.Exception.Message)"; throw }
Log "start InstallRoot=[$InstallRoot] NoElevate=$NoElevate KeepData=$KeepData KeepDrivers=$KeepDrivers"

function Test-Admin {
    $id = [Security.Principal.WindowsIdentity]::GetCurrent()
    (New-Object Security.Principal.WindowsPrincipal $id).IsInRole(
        [Security.Principal.WindowsBuiltinRole]::Administrator)
}

if (-not $NoElevate -and -not (Test-Admin)) {
    $scriptFile = if ($PSCommandPath) { $PSCommandPath } else { $MyInvocation.MyCommand.Path }
    $argList = @('-NoProfile', '-ExecutionPolicy', 'Bypass', '-File', "`"$scriptFile`"",
                 '-InstallRoot', "`"$InstallRoot`"", '-DataRoot', "`"$DataRoot`"")
    if ($KeepData)    { $argList += '-KeepData' }
    if ($KeepDrivers) { $argList += '-KeepDrivers' }
    Start-Process -FilePath 'powershell.exe' -ArgumentList $argList -Verb RunAs -Wait
    exit $LASTEXITCODE
}

Write-Host "Uninstalling $appDisplay"
Log "running elevated"

$startMenuFolder = Join-Path $env:ProgramData "Microsoft\Windows\Start Menu\Programs\$appDisplay"
Remove-Item $startMenuFolder -Recurse -Force -ErrorAction SilentlyContinue
Log "start-menu folder removed"

Remove-Item "HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$appName" `
    -Recurse -Force -ErrorAction SilentlyContinue
Log "ARP registry key removed"

if (-not $KeepDrivers) {
    Write-Host 'Removing FTDI drivers...'
    $ftdiInfs = @('ftdibus.inf', 'ftdiport.inf')
    try {
        foreach ($svc in @('FTDIBUS', 'FTSER2K')) {
            $s = Get-Service -Name $svc -ErrorAction SilentlyContinue
            if ($s -and $s.Status -ne 'Stopped') {
                Write-Host "  Stopping service $svc ..."
                Stop-Service -Name $svc -Force -ErrorAction SilentlyContinue
                Log "stopped service $svc"
            }
        }

        $manifestPath = Join-Path $InstallRoot 'ftdi-drivers.txt'
        $oemNames = @()
        if (Test-Path $manifestPath) {
            $oemNames = Get-Content -LiteralPath $manifestPath -ErrorAction SilentlyContinue | Where-Object { $_ -match '^oem\d+\.inf$' }
            Log "using FTDI driver manifest ($($oemNames -join ', ')) from $manifestPath"
        }

        if (-not $oemNames -or $oemNames.Count -eq 0) {
            Write-Host '  No FTDI driver manifest found - scanning installed drivers (fallback path, can take longer) ...'
            Log "no manifest found - falling back to pnputil /enum-drivers scan"
            $enumStart = Get-Date
            $pnpLines = (& pnputil /enum-drivers 2>&1) | ForEach-Object { "$_" }
            Log ("pnputil /enum-drivers finished in {0:N1}s" -f ((Get-Date) - $enumStart).TotalSeconds)
            $currentOem = $null
            foreach ($line in $pnpLines) {
                if ($line -match 'Published Name\s*:\s*(oem\d+\.inf)') {
                    $currentOem = $Matches[1]
                } elseif ($line -match 'Original Name\s*:\s*(\S+\.inf)') {
                    $origName = $Matches[1].Trim()
                    if ($ftdiInfs -contains $origName -and $currentOem) {
                        $oemNames += $currentOem
                        $currentOem = $null
                    }
                }
            }
        }

        foreach ($oemName in $oemNames) {
            Write-Host "  Removing $oemName ..."
            Log "removing driver $oemName"
            (& pnputil /delete-driver $oemName /uninstall /force 2>&1) |
                ForEach-Object { Log "  pnputil: $_" }
            if ($LASTEXITCODE -eq 0) {
                Write-Host "  Removed."
                Log "  removed OK"
            } else {
                Write-Warning "  pnputil exit $LASTEXITCODE for $oemName - driver may still be in use. Disconnect FTDI devices and retry."
                Log "  pnputil exit $LASTEXITCODE"
            }
        }
    } catch {
        Write-Warning "FTDI driver removal encountered an error: $($_.Exception.Message)"
        Log "FTDI removal error: $($_.Exception.Message)"
    }
}

$qtacProcessNames = @(
    'TAC', 'DeviceCatalog', 'TACConfigEditor', 'FTDICheck',
    'LITEProgrammer', 'TACDump', 'DevList', 'UpdateDeviceList'
)
foreach ($procName in $qtacProcessNames) {
    Get-Process -Name $procName -ErrorAction SilentlyContinue | ForEach-Object {
        try {
            $procPath = $_.Path
            if ($procPath -and $procPath.StartsWith($InstallRoot, [StringComparison]::OrdinalIgnoreCase)) {
                Write-Host "  Stopping running process $($_.ProcessName) (PID $($_.Id)) ..."
                Log "stopping process $($_.ProcessName) (PID $($_.Id))"
                Stop-Process -Id $_.Id -Force -ErrorAction SilentlyContinue
            }
        } catch {}
    }
}

Start-Sleep -Milliseconds 500

if (-not $KeepData) {
    Remove-Item $DataRoot -Recurse -Force -ErrorAction SilentlyContinue
    if (Test-Path $DataRoot) {
        Log "DataRoot still present after Remove-Item (access denied or files in use)"
    } else {
        Log "DataRoot removed"
    }
}

Set-Location $env:SystemRoot   # don't sit inside any directory being deleted

if (Test-Path $InstallRoot) {
    Remove-Item $InstallRoot -Recurse -Force -ErrorAction SilentlyContinue
}

if (Test-Path $InstallRoot) {
    Log "InstallRoot still present after initial Remove-Item; retrying"
    for ($i = 0; $i -lt 10 -and (Test-Path $InstallRoot); $i++) {
        Start-Sleep -Seconds 2
        Remove-Item $InstallRoot -Recurse -Force -ErrorAction SilentlyContinue
    }
}

if (Test-Path $InstallRoot) {
    Write-Warning "Could not fully remove: $InstallRoot. Please close any running QTAC applications and delete it manually."
    Write-Warning "If any FTDI/QTAC debug boards are connected via USB, disconnecting them may also help."
    Log "InstallRoot still present after retries - giving up"
} else {
    Log "InstallRoot removed"
}

Write-Host "$appDisplay uninstalled."
Log "done"