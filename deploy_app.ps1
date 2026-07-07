$qtbin = 'C:\Qt\6.11.1\msvc2022_64\bin'
$appDir = 'C:\ProdTools\qtac-refactor\build\Release'
$ftdiBin = 'C:\ProdTools\qtac-refactor\__Builds\x64\Release\bin'
$confDir = 'C:\ProdTools\qtac-refactor\configurations'

# Copy ftd2xx.dll next to exe
Copy-Item "$ftdiBin\ftd2xx.dll" $appDir -Force
Write-Output "Copied ftd2xx.dll"

# Copy devicelist.json and all tcnf configs so the app can find them in the exe directory
Copy-Item "$confDir\devicelist.json" $appDir -Force
Write-Output "Copied devicelist.json"
Copy-Item "$confDir\*.tcnf" $appDir -Force
Write-Output "Copied *.tcnf files"

# Run windeployqt to copy Qt DLLs
$windeployqt = "$qtbin\windeployqt.exe"
& $windeployqt --release "$appDir\qtac-app.exe"
Write-Output "windeployqt done, exit: $LASTEXITCODE"
