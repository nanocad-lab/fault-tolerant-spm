#include <stdint.h>
#include "ELF.h"
#include "MiscFuncs.h"
#include "Instruction.h"
#include <iostream>

ElfHeader::ElfHeader(ifstream* file, int elfHeaderStart, char* buff)
{
	//e_ident[5] is the endianness
	file->seekg(elfHeaderStart + 5);
	file->read(buff, sizeof(e_ident[5]));
	e_ident[5] = buff[0];

	//e_entry, offset by 24
	file->seekg(elfHeaderStart + 24);
	file->read(buff, sizeof(e_entry));
	e_entry = stringToNumericInstruction(buff, sizeof(e_entry), e_ident[5]);

	//e_phoff, offset by 28
	file->seekg(elfHeaderStart + 28);
	file->read(buff, sizeof(e_phoff));
	e_phoff = stringToNumericInstruction(buff, sizeof(e_phoff), e_ident[5]);

	//e_shoff, offset by 32
	file->seekg(elfHeaderStart + 32);
	file->read(buff, sizeof(e_shoff));
	e_shoff = stringToNumericInstruction(buff, sizeof(e_shoff), e_ident[5]);

	//e_phentsize, offset by 42
	file->seekg(elfHeaderStart + 42);
	file->read(buff, sizeof(e_phentsize));
	e_phentsize = stringToNumericInstruction(buff, sizeof(e_phentsize), e_ident[5]);

	//e_phnum, offset by 44
	file->seekg(elfHeaderStart + 44);
	file->read(buff, sizeof(e_phnum));
	e_phnum = stringToNumericInstruction(buff, sizeof(e_phnum), e_ident[5]);

	//e_phnum, offset by 46
	file->seekg(elfHeaderStart + 46);
	file->read(buff, sizeof(e_shentsize));
	e_shentsize = stringToNumericInstruction(buff, sizeof(e_shentsize), e_ident[5]);

	file->seekg(elfHeaderStart + 48);
	file->read(buff, sizeof(e_shnum));
	e_shnum = stringToNumericInstruction(buff, sizeof(e_shnum), e_ident[5]);
}

SectionHeader::SectionHeader(ifstream* file, int sectionHeaderStart, char* buff, char endianness)
{
	file->seekg(sectionHeaderStart);
	file->read(buff, sizeof(wordsh_name));
	wordsh_name = stringToNumericInstruction(buff, sizeof(wordsh_name), endianness);

	file->seekg(sectionHeaderStart + 4);
	file->read(buff, sizeof(wordsh_type));
	wordsh_type = stringToNumericInstruction(buff, sizeof(wordsh_type), endianness);

	file->seekg(sectionHeaderStart + 12);
	file->read(buff, sizeof(addrsh_addr));
	addrsh_addr = stringToNumericInstruction(buff, sizeof(addrsh_addr), endianness);

	file->seekg(sectionHeaderStart + 16);
	file->read(buff, sizeof(offsh_offset));
	offsh_offset = stringToNumericInstruction(buff, sizeof(offsh_offset), endianness);

	file->seekg(sectionHeaderStart + 20);
	file->read(buff, sizeof(wordsh_size));
	wordsh_size = stringToNumericInstruction(buff, sizeof(wordsh_size), endianness);

	file->seekg(sectionHeaderStart + 24);
	file->read(buff, sizeof(wordsh_link));
	wordsh_link = stringToNumericInstruction(buff, sizeof(wordsh_link), endianness);

	file->seekg(sectionHeaderStart + 28);
	file->read(buff, sizeof(wordsh_info));
	wordsh_info = stringToNumericInstruction(buff, sizeof(wordsh_info), endianness);

	file->seekg(sectionHeaderStart + 32);
	file->read(buff, sizeof(wordsh_addralign));
	wordsh_addralign = stringToNumericInstruction(buff, sizeof(wordsh_addralign), endianness);

	file->seekg(sectionHeaderStart + 36);
	file->read(buff, sizeof(wordsh_entsize));
	wordsh_entsize = stringToNumericInstruction(buff, sizeof(wordsh_entsize), endianness);
}


ProgramHeader::ProgramHeader(ifstream* file, int programHeaderStart, char* buff, char endianness)
{
	file->seekg(programHeaderStart);
	file->read(buff, sizeof(p_type));
	p_type = stringToNumericInstruction(buff, sizeof(p_type), endianness);
	
	file->seekg(programHeaderStart + 4);
	file->read(buff, sizeof(p_offset));
	p_offset = stringToNumericInstruction(buff, sizeof(p_offset), endianness);

	file->seekg(programHeaderStart + 8);
	file->read(buff, sizeof(p_vaddr));
	p_vaddr = stringToNumericInstruction(buff, sizeof(p_vaddr), endianness);

	file->seekg(programHeaderStart + 12);
	file->read(buff, sizeof(p_padder));
	p_padder = stringToNumericInstruction(buff, sizeof(p_padder), endianness);
	
	file->seekg(programHeaderStart + 16);
	file->read(buff, sizeof(p_filesz));
	p_filesz = stringToNumericInstruction(buff, sizeof(p_filesz), endianness);
	
	file->seekg(programHeaderStart + 20);
	file->read(buff, sizeof(p_memsz));
	p_memsz = stringToNumericInstruction(buff, sizeof(p_memsz), endianness);
	
	file->seekg(programHeaderStart + 24);
	file->read(buff, sizeof(p_flags));
	p_flags = stringToNumericInstruction(buff, sizeof(p_flags), endianness);
	
	file->seekg(programHeaderStart + 28);
	file->read(buff, sizeof(p_align));
	p_align = stringToNumericInstruction(buff, sizeof(p_align), endianness);
}