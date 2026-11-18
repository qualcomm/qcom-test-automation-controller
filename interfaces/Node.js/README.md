# Node.js NAPI Bindings for QTAC

Node.js native bindings for the Qualcomm Test Automation Controller (QTAC) library.

## Installation

```bash
npm install @qualcomm/qtac
```

## Prerequisites

- Node.js 18.0.0 or higher
- CMake 3.22 or higher
- Qt 6.9 or higher
- FTDI D2XX drivers installed
- TACDev library built (part of QTAC workspace)

## Quick Start

```javascript
const QTAC = require('@qualcomm/qtac');

// Get library version
console.log('QTAC Version:', QTAC.getVersion());

// List available devices
const devices = QTAC.listDevices();
console.log(`Found ${devices.length} device(s)`);

// Open a device
if (devices.length > 0) {
  const device = QTAC.openDevice(devices[0].port);
  
  // Get device information
  console.log('Device Name:', device.getName());
  console.log('Firmware:', device.getFirmwareVersion());
  console.log('Hardware:', device.getHardware());
  
  // Control device
  await device.setBatteryState(true);
  await device.powerOn();
  
  // Close when done
  device.close();
}
```

## API Documentation

### QTAC Static Methods

- `QTAC.getVersion()` - Get QTAC version string
- `QTAC.getTACVersion()` - Get TAC version string
- `QTAC.getDeviceCount()` - Get number of connected devices
- `QTAC.listDevices()` - Get array of DeviceInfo objects
- `QTAC.openDevice(portNameOrSerial)` - Open device and return TACDevice instance
- `QTAC.setLogging(enabled)` - Enable/disable logging

### TACDevice Instance Methods

#### Device Information
- `getName()` - Get device name
- `getFirmwareVersion()` - Get firmware version
- `getHardware()` - Get hardware type
- `getHardwareVersion()` - Get hardware version
- `getUUID()` - Get device UUID

#### Device Control (async)
- `setBatteryState(state)` / `getBatteryState()`
- `setUsb0(state)` / `getUsb0State()`
- `setUsb1(state)` / `getUsb1State()`
- `setPowerKey(state)` / `getPowerKeyState()`
- `setVolumeUp(state)` / `getVolumeUpState()`
- `setVolumeDown(state)` / `getVolumeDownState()`
- And 30+ more control functions...

#### Quick Commands (async)
- `powerOn()` - Execute power on sequence
- `powerOff()` - Execute power off sequence
- `bootToFastboot()` - Boot device to fastboot mode
- `bootToEDL()` - Boot device to EDL mode
- `bootToUEFI()` - Boot device to UEFI menu

#### Dynamic Commands
- `getCommandCount()` - Get number of available commands
- `getCommand(index)` - Get command metadata
- `sendCommand(command, state)` - Send command to device (async)

#### Script Variables
- `getScriptVariableCount()` - Get number of script variables
- `getScriptVariable(index)` - Get variable definition
- `updateScriptVariable(name, value)` - Update variable value

#### Configuration
- `setName(name)` - Rename device
- `getResetCount()` - Get hardware reset count
- `clearResetCount()` - Clear reset counter

#### Cleanup
- `close()` - Close device handle

## Examples

See the `examples/` directory for more usage examples:
- `list-devices.js` - Device enumeration
- `device-control.js` - Basic device control
- `send-commands.js` - Dynamic command execution

## TypeScript Support

This package includes TypeScript definitions. No additional @types package is needed.

```typescript
import QTAC, { TACDevice, DeviceInfo } from '@qualcomm/qtac';

const devices: DeviceInfo[] = QTAC.listDevices();
const device: TACDevice = QTAC.openDevice(devices[0].port);
```

## Building from Source

On Windows ARM64, open a PowerShell terminal with the Qt MSVC ARM64
installation selected before running the Node.js build:

```powershell
$env:QTBIN = "C:\Qt\6.9.3\msvc2022_arm64\bin"
$env:CMAKE_PREFIX_PATH = Split-Path $env:QTBIN -Parent
```

Replace the Qt version/path with the installation on your machine. The Qt
configuration file must exist at:

```text
C:\Qt\6.9.3\msvc2022_arm64\lib\cmake\Qt6\Qt6Config.cmake
```

```powershell
# Install dependencies
npm install

# Build native module
npm run build:native

# Build TypeScript
npm run build

# Run tests
npm run test:unit
npm run test:native-smoke

# Run an example (requires a TAC device)
node examples/list-devices.js
```

The native smoke test checks that the addon loads, initializes, reports its
versions, and enumerates devices. It does not open device handles by default;
pass `--open-devices` to include handle and device-information checks. On Linux,
set `LD_LIBRARY_PATH` before starting Node so it includes the native runtime
library directory (`__Builds/Linux/Release/lib`) and the Qt `lib` directory.

## Platform Support

- Windows x64
- Windows ARM64
- Linux x86_64

## License

BSD-3-Clause

Copyright (c) Qualcomm Technologies, Inc.

## Support

For issues and questions, please visit:
https://github.com/qualcomm/qcom-test-automation-controller
