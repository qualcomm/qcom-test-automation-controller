. "$PSScriptRoot\env_setup.ps1"

$appDir  = "$PSScriptRoot\build\Release"
$ftdiBin = "$PSScriptRoot\__Builds\x64\Release\bin"
$confDir = "$PSScriptRoot\configurations"

# Copy ftd2xx.dll next to exe
Copy-Item "$ftdiBin\ftd2xx.dll" $appDir -Force
Write-Output "Copied ftd2xx.dll"

# Copy devicelist.json and all tcnf configs so the app can find them in the exe directory
Copy-Item "$confDir\devicelist.json" $appDir -Force
Write-Output "Copied devicelist.json"
Copy-Item "$confDir\*.tcnf" $appDir -Force
Write-Output "Copied *.tcnf files"
Copy-Item "$confDir\DefaultScript.txt" $appDir -Force
Write-Output "Copied DefaultScript.txt"

# Run windeployqt to copy Qt DLLs
$windeployqt = "$qtRoot\bin\windeployqt.exe"
& $windeployqt --release "$appDir\qtac-app.exe"
Write-Output "windeployqt done, exit: $LASTEXITCODE"
