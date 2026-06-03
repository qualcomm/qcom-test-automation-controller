# External Dependencies

## FTDI D2XX Library (required)

The FTDI D2XX library is a proprietary driver SDK from FTDI Chip. It is a required
build dependency but **cannot be downloaded automatically** — the FTDI website
(`ftdichip.com`) is protected by Cloudflare, which blocks automated tools such as
CMake, curl, and wget.

You must download the archive manually using a browser **before running CMake**.

### Download

Go to the FTDI D2XX drivers page and download the latest version for your platform:

**https://ftdichip.com/drivers/d2xx-drivers/**

| Platform | Archive name pattern |
|----------|----------------------|
| Windows  | `CDM-vX.XX.XX-WHQL-Certified.zip` |
| Linux x86_64 | `libftd2xx-linux-x86_64-X.X.XX.tgz` |

### Build instructions

After downloading, pass the archive path to CMake:

**Windows:**
```
cmake -S . -B build -DFTDI_ARCHIVE_PATH=C:\path\to\CDM-vX.XX.XX-WHQL-Certified.zip
```

**Linux:**
```
cmake -S . -B build -DFTDI_ARCHIVE_PATH=/path/to/libftd2xx-linux-x86_64-X.X.XX.tgz
```

CMake will extract the library into `__Builds/` on the first run. Subsequent builds
do not need `-DFTDI_ARCHIVE_PATH` unless you clean the `__Builds/` directory.

### Notes

- The archive is not committed to this repository (proprietary, binary).
- FTDI's license terms are included in the downloaded archive.
- If you are setting up multiple machines, you can copy the downloaded archive to
  each machine rather than downloading it again.