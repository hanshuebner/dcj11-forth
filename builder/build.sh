#!/bin/sh

# This build script prepares and assembles FORTH for the PDP-11. It creates an RT-11 filesystem
# image, copies the source and binary files, then uses the MACRO-11 assembler on RT-11 to compile
# the FORTH source code. The assemble.py script handles the automation of the assembly process
# by managing the interaction between the host system and RT-11 running in the SIMH emulator.

set -e

PDPFS="pdpfs -i exchange.img"

$PDPFS mkfs rx02 rt11
$PDPFS cp data/FORTH.MAC FORTH.MAC
$PDPFS cp data/FORTH.COM FORTH.COM
python3 assemble.py
$PDPFS cp FORTH.LST data/FORTH.LST
$PDPFS cp FORTH.LDA ./FORTH.LDA
rm exchange.img*
./lda2bin < FORTH.LDA > data/forth.bin
rm -f FORTH.LDA
