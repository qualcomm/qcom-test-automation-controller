# External Dependencies

## FTDI D2XX Library (required)

The FTDI D2XX library is a proprietary driver SDK from FTDI Chip. It is a required
build dependency but **cannot be downloaded automatically** — the FTDI website
(`ftdichip.com`) is protected by Cloudflare, which blocks automated tools such as
CMake, curl, and wget.

You must download the archive manually using a browser **before running CMake**.

### Download

| Platform | File | URL |
|----------|------|-----|
| Windows  | `CDM-v2.12.36.20-WHQL-Certified.zip` | https://ftdichip.com/wp-content/uploads/2025/03/CDM-v2.12.36.20-WHQL-Certified.zip |
| Linux    | `libftd2xx-linux-x86_64-1.4.33.tgz`  | https://ftdichip.com/wp-content/uploads/2025/03/libftd2xx-linux-x86_64-1.4.33.tgz  |

### Build instructions

After downloading, pass the archive path to CMake:

**Windows:**
```
cmake -S . -B build -DFTDI_ARCHIVE_PATH=C:\path\to\CDM-v2.12.36.20-WHQL-Certified.zip
```

**Linux:**
```
cmake -S . -B build -DFTDI_ARCHIVE_PATH=/path/to/libftd2xx-linux-x86_64-1.4.33.tgz
```

CMake will extract the library into `__Builds/` on the first run. Subsequent builds
do not need `-DFTDI_ARCHIVE_PATH` unless you clean the `__Builds/` directory.

### Notes

- The archive is not committed to this repository (proprietary, binary).
- FTDI's license terms are included in the downloaded archive.
- If you are setting up multiple machines, you can copy the downloaded archive to
  each machine rather than downloading it again.