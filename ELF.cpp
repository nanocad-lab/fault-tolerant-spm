#include <stdint.h>
#include "ELF.h"
#include "MiscFuncs.h"
#include "Instruction.h"

ElfHeader::ElfHeader(ifstream* file, int elfHeaderStart, char* buff)
{

	//e_version, offset by 20
	file->seekg(elfHeaderStart + 20);
	file->read(buff, 4);
	changeEndian(buff, 32, 2);
	e_version = stringToIntInstruction(buff, 32);

	//e_entry, offset by 24
	file->seekg(elfHeaderStart + 24);
	file->read(buff, 4);
	changeEndian(buff, 32, 2);
	e_entry = stringToIntInstruction(buff, 32);

	//e_phoff, offset by 28
	file->seekg(elfHeaderStart + 28);
	file->read(buff, 4);
	changeEndian(buff, 32, 2);
	e_phoff = stringToIntInstruction(buff, 32);

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