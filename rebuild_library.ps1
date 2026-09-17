. "$PSScriptRoot\env_setup.ps1"
cmake --build "$PSScriptRoot\build" --config Debug --target qtac-core TACDev qtac-app 2>&1
exit $LASTEXITCODE
