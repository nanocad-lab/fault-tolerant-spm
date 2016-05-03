#!/usr/bin/env bash
# This is a script that runs the entire Fault-Tolerant Scratchpad system. This script is not finished; further information about target board is required for arm-none-eabi-gcc options.

#Build files for the ARM Cortex-M3 into an ELF program
arm-none-eabi-gcc -nostdlib -mcpu=cortex-m3 -mthumb

alternative?
arm-none-eabi-as 
arm-none-eabi-ld -Ttext-segment=0x000000


#To dump an arm file
arm-none-eabi-objdump -b binary -marm -D name.bin -Mforce-thumb

#Run the Fault-Tolerant program on the ELF program
fault-tolerant-spm input -m memory -o modifiedelf

#Convert into BIN for ARM Cortex-M3 board
arm-none-eabi-objcopy -B binary modifiedelf output 

