Getting Started
==================================================
Download the following tools:

Keil:
--------------------------------------------------
Download Keil from: 
https://www.keil.com/download/product/

Download and read the pdf in the following link to setup keil for mbed LPC1768 development:
http://www.keil.com/appnotes/docs/apnt_207.asp

GNU Tools for ARM Embedded Processors:
--------------------------------------------------
Download this tool from: 
https://launchpad.net/gcc-arm-embedded/+download

Export to GCC ARM Embedded
--------------------------------------------------
The following shows how to get the bin file to put
on the mbed from the ELF.
https://developer.mbed.org/handbook/Exporting-to-GCC-ARM-Embedded



Manuals:
==================================================
ARM v7-M:
-------------------------------------------------- 
http://www.telecom.uff.br/~marcos/uP/ARMv7_Ref.pdf

https://web.eecs.umich.edu/~prabal/teaching/eecs373-f11/readings/ARMv7-M_ARM.pdf

ELF:
--------------------------------------------------
http://flint.cs.yale.edu/cs422/doc/ELF_Format.pdf

Useful reference for ARM-specific elf field values
http://infocenter.arm.com/help/topic/com.arm.doc.dui0101a/DUI0101A_Elf.pdf




Additional Miscellaneous Information:
==================================================
Addresses.txt is a text file with hex representations of 32-bit addresses.
Each 32-bit address is 8 hex characters prefixed by 0x.
Each address is separated from other addresses by newlines or whitespaces.

e.g.
0x040302AF
0x28EA1423
0x00FF0243

This program is operating under the assumption that the ELF file is little endian.
This is because the ARM LPC1768, our target microcontroller, is little endian.
In normal situations, the ELF file is the same endianness as the target machine.
ARM specifications store instructions as little endian; memory may be either.

The function name "stringToIntInstruction" is a bit misleading.
It just takes a byte buffer and stores it in a data type of appropriate size.