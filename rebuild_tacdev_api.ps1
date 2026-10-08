. "$PSScriptRoot\env_setup.ps1"
cmake --build "$PSScriptRoot\build" --config Debug --target test_tacdev_api
exit $LASTEXITCODE
