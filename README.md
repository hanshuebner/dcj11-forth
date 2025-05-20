# PDP-11 FORTH Development Environment

This repository contains tools for developing and running FORTH on a PDP-11 DCJ11 Single Board Computer. It includes:

1. A Docker-based RT-11 environment for assembling FORTH.MAC using the MACRO-11 assembler
2. A binary file uploader for transferring the assembled program to the DCJ11 SBC via serial port

## Requirements

 - Docker for running the RT-11 assembly environment
 - Python 3.6 or higher
 - pyserial library for serial port access
 - tqdm library for progress reporting

## Installation

 - Install the required Python dependencies:
```bash
pip install -r requirements.txt
```
 - Ensure Docker is installed and running on your system.

## Quick Start

 - Build the FORTH binary:
```bash
./assemble.sh
```
This will create `forth.bin` using RT-11 running in SIMH inside a Docker container.

 - Upload the binary to your DCJ11 SBC:
```bash
python odt-uploader.py /dev/tty.usbserial-110 forth.bin 1000
```
Note: Adjust the serial port device as needed for your system.

## Useful Links

 - [Starting FORTH](https://www.forth.com/starting-forth/)
 - [Systems Guide to FIG FORTH](https://www.forth.org/OffeteStore/1010_SystemsGuideToFigForth.pdf)
 - [RT-11 V4.0 User's Guide](https://bitsavers.org/pdf/dec/pdp11/rt11/v4.0_Mar80/2a/AA-5279B-TC_RT-11_V4.0_System_Users_Guide_Mar80.pdf)
 - [PDP-11 MACRO-11 Reference Manual](https://bitsavers.org/pdf/dec/pdp11/rt11/v4.0_Mar80/3a/AA-5075B-TC_PDP-11_MACRO-11_Language_Reference_Manual_Jan80.pdf)
 - [J-11 Programmer's Reference](http://www.bitsavers.org/pdf/dec/pdp11/j11/J-11_Programmers_Reference_Jan82.pdf)
 - [DCJ11 Microprocessor User's Guide](http://www.bitsavers.org/pdf/dec/pdp11/1173/EK-DCJ11-UG-PRE_J11ug_Oct83.pdf)

## The Assembly Process

The `assemble.sh` script:

 - Builds a Docker container with RT-11 and SIMH
 - Mounts the local directory containing FORTH.MAC
 - Runs the MACRO-11 assembler to create forth.bin
 - Exits the container, leaving forth.bin in your local directory

## Binary Uploader

The `odt-uploader.py` script uploads binary files to a PDP-11 through a serial port while the ODT monitor is running. It handles the communication protocol and ensures proper byte ordering for the PDP-11 architecture.

### Usage

```bash
python odt-uploader.py <serial_port> <binary_file> <start_address> [-v]
```

Where:

 - `<serial_port>` is the serial port device (e.g., `/dev/ttyUSB0` on Linux, `COM1` on Windows)
 - `<binary_file>` is the path to the binary file to upload
 - `<start_address>` is the octal start address where the file should be loaded
 - `-v` or `--verbose` enables detailed logging of serial communication

### Features

- Configures serial port for 38400 bps, 8N1
- Handles PDP-11 little-endian byte ordering
- Verifies character echo from ODT
- Provides progress updates during upload
- Handles timeouts and communication errors
- Ensures even number of bytes by padding if necessary
- Optional verbose logging of all serial communication

### Verbose Logging

When the `-v` flag is used, the tool will log:

 - All bytes sent and received in hex and ASCII format
 - Detailed information about each word being sent
 - File size and padding information
 - Serial port configuration details
 - Progress updates during upload

Example verbose output:
```
12:34:56 - INFO - File size: 1024 bytes
12:34:56 - INFO - Opened serial port /dev/ttyUSB0 at 38400 bps, 8N1
12:34:56 - INFO - Waiting for initial prompt...
12:34:56 - DEBUG - RX: 40 | @
12:34:56 - DEBUG - TX: 0d | \r
12:34:56 - DEBUG - RX: 0d | \r
12:34:56 - DEBUG - RX: 40 | @
12:34:56 - DEBUG - Sending word 000123 (decimal: 83)
12:34:56 - DEBUG - TX: 31 | 1
12:34:56 - DEBUG - RX: 31 | 1
...
```

### Error Handling

The tool will:

 - Verify character echo from ODT
 - Timeout if expected responses are not received
 - Report any communication errors
 - Exit with status code 1 if any errors occur 
