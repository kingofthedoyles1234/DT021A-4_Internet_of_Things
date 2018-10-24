#!/bin/bash
export PATH=/usr/local/mbed_image/gcc-arm-none-eabi/bin:$PATH
make
cd ..
echo "Copy the file mbed_skeleton/BUILD/mbed_project-combined.hex to the target device"
