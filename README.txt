Getting Started
==================================================

Running on Linux with GCC

Get GNU ARM Embedded Toolchain
--------------------------------------------------
Follow the instructions on:
https://launchpad.net/~terry.guo/+archive/ubuntu/gcc-arm-embedded

Note: On step 4, run the command
sudo apt-get install gcc-arm-none-eabi=4.9.3.2015q2-1trusty1

This will add ARM Embedded GCC to your PATH as arm-none-eabi-gcc

Note: Installation package is gcc-arm-none-eabi but the gcc is run with arm-none-eabi-gcc


Export mbed program to GCC (ARM Embedded)
--------------------------------------------------
The following shows how to get the bin file to put
on the mbed from the ELF.
https://developer.mbed.org/handbook/Exporting-to-GCC-ARM-Embedded
Unzip the downloaded folder and run make inside it to get an ELF, bin, and hex


Get QEMU and set it up for ARM Embedded
--------------------------------------------------
sudo apt-get install qemu-system-arm


Running QEMU
--------------------------------------------------
qemu-system-arm -machine none -cpu cortex-m3


Run program
--------------------------------------------------
Compile with Visual Studio for Windows or Eclipse/Make on Linux
Makefile is provided with the rest of the source code

The program takes no inputs; only the ELF you want to modify (named ELF


Convert modified ELF to bin
--------------------------------------------------
Run the command
arm-none-eabi-objcopy -O binary <file_name>.elf <file_name>.bin


Manuals:
==================================================
ARM v7-M:
-------------------------------------------------- 
ARM v7-M Architecture Application Level Reference Manual
http://www.telecom.uff.br/~marcos/uP/ARMv7_Ref.pdf

ARM v7-M Architecture Reference Manual Errata markup
https://web.eecs.umich.edu/~prabal/teaching/eecs373-f11/readings/ARMv7-M_ARM.pdf

ELF:
--------------------------------------------------
Tool Interface Standards Specification
http://flint.cs.yale.edu/cs422/doc/ELF_Format.pdf

Reference for ARM-specific ELF field values
http://infocenter.arm.com/help/topic/com.arm.doc.dui0101a/DUI0101A_Elf.pdf

Useful infographic for ELF files
https://code.google.com/p/corkami/wiki/ELF101
