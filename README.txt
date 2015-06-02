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



Additional Miscellaneous Information:
==================================================
Addresses.hex is a text file with hex representations of 32-bit addresses.
Each 32-bit address is 8 hex characters, separated in the middle by one space.
Each address is separated from other addresses by one space.

e.g.
0403 02AF 28EA 1423 00FF 0243

This is a total of four addresses.


TODO
==================================================
Notes:
Adjust where for loop starts and ends to only look for addresses in code segment.
ELF part of the code was just written to figure out where code segment is.
Based on assumption that there was only code in the file, so iterator in for loops such as input eip is not actually the eip of the code but location of char in in the input file (from Muzammil)

Fixes:
Parser for "addresses" text file should parse for format 0x12345678\n0x12345678\n...
ELF file parser cannot assume the absence of ANY fields.