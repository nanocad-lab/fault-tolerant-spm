#include <stdint.h>
#include "ELF.h"
#include "MiscFuncs.h"
#include "Instruction.h"

ElfHeader::ElfHeader(ifstream* file, int elfHeaderStart, char* buff)
{
	//ElfHeader offsets and format
	/* typedef struct {
		uint8_t		e_ident[ELF_NIDENT = 16];	+0
		Elf32_Half	e_type;						+16
		Elf32_Half	e_machine;					+18
		Elf32_Word	e_version;					+20
		Elf32_Addr	e_entry;					+24
		Elf32_Off	e_phoff;					+28
		Elf32_Off	e_shoff;					+32
		Elf32_Word	e_flags;					+36
		Elf32_Half	e_ehsize;					+40
		Elf32_Half	e_phentsize;				+42
		Elf32_Half	e_phnum;					+44
		Elf32_Half	e_shentsize;				+46
		Elf32_Half	e_shnum;					+48
		Elf32_Half	e_shstrndx;					+50
	} Elf32_Ehdr; */

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
	//SectionHeader offsets and format
	/*typedef struct {
	Elf32_Word	sh_name;		+0
	Elf32_Word	sh_type;		+4
	Elf32_Word	sh_flags;		+8
	Elf32_Addr	sh_addr;		+12
	Elf32_Off	sh_offset;		+16
	Elf32_Word	sh_size;		+20
	Elf32_Word	sh_link;		+24
	Elf32_Word	sh_info;		+28
	Elf32_Word	sh_addralign;	+32
	Elf32_Word	sh_entsize;		+36
	} Elf32_Shdr;*/

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
	/*typedef struct {
		Elf32_Word		p_type;		+0
		Elf32_Off		p_offset;	+4
		Elf32_Addr		p_vaddr;	+8
		Elf32_Addr		p_paddr;	+12
		Elf32_Word		p_filesz;	+16
		Elf32_Word		p_memsz;	+20
		Elf32_Word		p_flags;	+24
		Elf32_Word		p_align;	+28
	} Elf32_Phdr;*/

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