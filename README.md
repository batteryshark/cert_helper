# cert_helper

A native Windows library for managing CA certificates in the system's root certificate store.

## Purpose

`cert_helper` provides a simple C API for certificate operations that would otherwise require complicated managed code:
- Check if a certificate is installed in the Windows root store
- Install a certificate to the Windows root store
- Remove a certificate from the Windows root store

## Requirements

- Windows operating system
- Administrator privileges (for installing/removing certificates)
- For building: MinGW-w64 cross-compiler

## Build Instructions

```bash
# Build both 32-bit and 64-bit versions
make all

# Build only 32-bit version
make win32

# Build only 64-bit version
make win64

# Clean build artifacts
make clean
```

Compiled binaries will be placed in `bin/win32` and `bin/win64` directories.

## API

The library exports three main functions:

```c
// Check if a certificate is loaded in the system root store
BOOL IsCACertLoaded(const wchar_t* path_to_ca_cer);

// Install a certificate to the system root store
BOOL InstallCACert(const wchar_t* path_to_ca_cer);

// Remove a certificate from the system root store
BOOL RemoveCACert(const wchar_t* path_to_ca_cer);
```

All functions take a wide-character path to a .cer certificate file and return TRUE on success or FALSE on failure.

## Test Executable

A test executable is also built to demonstrate the library's functionality. Running it will:
1. Install a test certificate (mitmproxy-ca-cert.cer)
2. Verify it was installed
3. Remove the certificate

## License

See [LICENSE.md](LICENSE.md) for details.