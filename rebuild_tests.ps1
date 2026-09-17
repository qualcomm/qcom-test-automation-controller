. "$PSScriptRoot\env_setup.ps1"
cmake --build "$PSScriptRoot\build" --config Debug --target test_coders_commands test_hardware_ftdi test_hardware_psoc
exit $LASTEXITCODE
