#include <stdint.h>
#include "ELF.h"
#include "MiscFuncs.h"
#include "Instruction.h"

ElfHeader::ElfHeader(ifstream* file, int elfHeaderStart, char* buff)
{

	//e_entry, offset by 24
	file->seekg(elfHeaderStart + 24);
	file->read(buff, sizeof(e_entry));
	changeEndian(buff);
	e_entry = stringToIntInstruction(buff, 32); //this function needs to be fixed.

	//e_phoff, offset by 28
	file->seekg(elfHeaderStart + 28);
	file->read(buff, sizeof(e_phoff));
	changeEndian(buff);
	e_phoff = stringToIntInstruction(buff, 32);

	//e_shoff, offset by 32
	file->seekg(elfHeaderStart + 32);
	file->read(buff, sizeof(e_shoff));
	changeEndian(buff);
	e_shoff = stringToIntInstruction(buff, 32);

	//e_phentsize, offset by 42
	file->seekg(elfHeaderStart + 42);
	file->read(buff, sizeof(e_phentsize));
	changeEndian(buff);
	e_phentsize = stringToIntInstruction(buff);

	file->seekg(elfHeaderStart + 44);
	file->read(buff, sizeof(e_phnum));
	changeEndian(buff);
	e_phnum = stringToIntInstruction(buff);

	file->seekg(elfHeaderStart + 46);
	file->read(buff, sizeof(e_shentsize));
	changeEndian(buff);
	e_shentsize = stringToIntInstruction(buff);

	file->seekg(elfHeaderStart + 48);
	file->read(buff, sizeof(e_shnum));
	changeEndian(buff);
	e_shnum = stringToIntInstruction(buff);

}

SectionHeader::SectionHeader(ifstream* file, int sectionHeaderStart, char* buff)
{

	file->seekg(sectionHeaderStart);
	file->read(buff, 4);
	changeEndian(buff);
	wordsh_name = stringToIntInstruction(buff, 32);

	file->seekg(sectionHeaderStart+4);
	file->read(buff, 4);
	changeEndian(buff);
	wordsh_type = stringToIntInstruction(buff, 32);

	file->seekg(sectionHeaderStart+12);
	file->read(buff, 4);
	changeEndian(buff);
	addrsh_addr = stringToIntInstruction(buff, 32);

	file->seekg(sectionHeaderStart+16);
	file->read(buff, 4);
	changeEndian(buff);
	offsh_offset = stringToIntInstruction(buff, 32);

	file->seekg(sectionHeaderStart+20);
	file->read(buff, 4);
	changeEndian(buff);
	wordsh_size = stringToIntInstruction(buff, 32);

	file->seekg(sectionHeaderStart+24);
	file->read(buff, 4);
	changeEndian(buff);
	wordsh_link = stringToIntInstruction(buff, 32);

	file->seekg(sectionHeaderStart+28);
	file->read(buff, 4);
	changeEndian(buff);
	wordsh_info = stringToIntInstruction(buff, 32);

	file->seekg(sectionHeaderStart+32);
	file->read(buff, 4);
	changeEndian(buff);
	wordsh_addralign = stringToIntInstruction(buff, 32);

	file->seekg(sectionHeaderStart+36);
	file->read(buff, 4);
	changeEndian(buff);
	wordsh_entsize = stringToIntInstruction(buff, 32);
}


ProgramHeader::ProgramHeader(ifstream* file, int programHeaderStart, char* buff)
{

	file->seekg(programHeaderStart);
	file->read(buff, 4);
	changeEndian(buff);
	p_type = stringToIntInstruction(buff, 32);
	
	file->seekg(programHeaderStart+4);
	file->read(buff, 4);
	changeEndian(buff);
	p_offset = stringToIntInstruction(buff, 32);

	file->seekg(programHeaderStart+8);
	file->read(buff, 4);
	changeEndian(buff);
	p_vaddr = stringToIntInstruction(buff, 32);

	file->seekg(programHeaderStart+12);
	file->read(buff, 4);
	changeEndian(buff);
	p_padder = stringToIntInstruction(buff, 32);
	
	file->seekg(programHeaderStart+16);
	file->read(buff, 4);
	changeEndian(buff);
	p_filesz = stringToIntInstruction(buff, 32);
	
	file->seekg(programHeaderStart+20);
	file->read(buff, 4);
	changeEndian(buff);
	p_memsz = stringToIntInstruction(buff, 32);
	
	file->seekg(programHeaderStart+24);
	file->read(buff, 4);
	changeEndian(buff);
	p_flags = stringToIntInstruction(buff, 32);
	
	file->seekg(programHeaderStart+28);
	file->read(buff, 4);
	changeEndian(buff);
	p_align = stringToIntInstruction(buff, 32);
	
}