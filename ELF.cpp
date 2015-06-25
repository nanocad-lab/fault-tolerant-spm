#include <stdint.h>
#include "ELF.h"
#include "MiscFuncs.h"
#include "Instruction.h"

ElfHeader::ElfHeader(ifstream* file, int elfHeaderStart, char* buff)
{

	//e_entry, offset by 24
	file->seekg(elfHeaderStart + 24);
	file->read(buff, sizeof(e_entry));
	changeEndian(buff, 32, 2);
	e_entry = stringToIntInstruction(buff, 32); //this function needs to be fixed.

	//e_phoff, offset by 28
	file->seekg(elfHeaderStart + 28);
	file->read(buff, sizeof(e_phoff));
	changeEndian(buff, 32, 2);
	e_phoff = stringToIntInstruction(buff, 32);

	//e_shoff, offset by 32
	file->seekg(elfHeaderStart + 32);
	file->read(buff, sizeof(e_shoff));
	changeEndian(buff, 32, 2);
	e_shoff = stringToIntInstruction(buff, 32);

	//e_phentsize, offset by 42
	file->seekg(elfHeaderStart + 42);
	file->read(buff, sizeof(e_phentsize));
	changeEndian(buff, 16);
	e_phentsize = stringToIntInstruction(buff);

	file->seekg(elfHeaderStart + 44);
	file->read(buff, sizeof(e_phnum));
	changeEndian(buff, 16);
	e_phnum = stringToIntInstruction(buff);

	file->seekg(elfHeaderStart + 46);
	file->read(buff, sizeof(e_shentsize));
	changeEndian(buff, 16);
	e_shentsize = stringToIntInstruction(buff);

	file->seekg(elfHeaderStart + 48);
	file->read(buff, sizeof(e_shnum));
	changeEndian(buff, 16);
	e_shnum = stringToIntInstruction(buff);

}

SectionHeader::SectionHeader(ifstream* file, int sectionHeaderStart, char* buff)
{

	file->seekg(sectionHeaderStart);
	file->read(buff, 4);
	changeEndian(buff, 32, 2);
	wordsh_name = stringToIntInstruction(buff, 32);

	file->seekg(sectionHeaderStart+4);
	file->read(buff, 4);
	changeEndian(buff, 32, 2);
	wordsh_type = stringToIntInstruction(buff, 32);

	file->seekg(sectionHeaderStart+12);
	file->read(buff, 4);
	changeEndian(buff, 32, 2);
	addrsh_addr = stringToIntInstruction(buff, 32);

	file->seekg(sectionHeaderStart+16);
	file->read(buff, 4);
	changeEndian(buff, 32, 2);
	offsh_offset = stringToIntInstruction(buff, 32);

	file->seekg(sectionHeaderStart+20);
	file->read(buff, 4);
	changeEndian(buff, 32, 2);
	wordsh_size = stringToIntInstruction(buff, 32);

	file->seekg(sectionHeaderStart+24);
	file->read(buff, 4);
	changeEndian(buff, 32, 2);
	wordsh_link = stringToIntInstruction(buff, 32);

	file->seekg(sectionHeaderStart+28);
	file->read(buff, 4);
	changeEndian(buff, 32, 2);
	wordsh_info = stringToIntInstruction(buff, 32);

	file->seekg(sectionHeaderStart+32);
	file->read(buff, 4);
	changeEndian(buff, 32, 2);
	wordsh_addralign = stringToIntInstruction(buff, 32);

	file->seekg(sectionHeaderStart+36);
	file->read(buff, 4);
	changeEndian(buff, 32, 2);
	wordsh_entsize = stringToIntInstruction(buff, 32);
}


ProgramHeader::ProgramHeader(ifstream* file, int programHeaderStart, char* buff)
{

	file->seekg(programHeaderStart);
	file->read(buff, 4);
	changeEndian(buff, 32, 2);
	p_type = stringToIntInstruction(buff, 32);
	
	file->seekg(programHeaderStart+4);
	file->read(buff, 4);
	changeEndian(buff, 32, 2);
	p_offset = stringToIntInstruction(buff, 32);

	file->seekg(programHeaderStart+8);
	file->read(buff, 4);
	changeEndian(buff, 32, 2);
	p_vaddr = stringToIntInstruction(buff, 32);

	file->seekg(programHeaderStart+12);
	file->read(buff, 4);
	changeEndian(buff, 32, 2);
	p_padder = stringToIntInstruction(buff, 32);
	
	file->seekg(programHeaderStart+16);
	file->read(buff, 4);
	changeEndian(buff, 32, 2);
	p_filesz = stringToIntInstruction(buff, 32);
	
	file->seekg(programHeaderStart+20);
	file->read(buff, 4);
	changeEndian(buff, 32, 2);
	p_memsz = stringToIntInstruction(buff, 32);
	
	file->seekg(programHeaderStart+24);
	file->read(buff, 4);
	changeEndian(buff, 32, 2);
	p_flags = stringToIntInstruction(buff, 32);
	
	file->seekg(programHeaderStart+28);
	file->read(buff, 4);
	changeEndian(buff, 32, 2);
	p_align = stringToIntInstruction(buff, 32);
	
}