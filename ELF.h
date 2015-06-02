#ifndef ELF_INCLUDED
#define ELF_INCLUDED

#include <stdint.h> //for exact specified sizes
#include <fstream>
//#include <iostream>

using namespace std;

struct ElfHeader
{
	ElfHeader(ifstream* file, int elfHeaderStart, char* buff);
	char e_ident[16];
	short e_type;
	short e_machine;
	int	e_version;
	int	e_entry;
	int	e_phoff;
	int	e_shoff;
	int	e_flags;
	short e_ehsize;
	short e_phentsize;
	short e_phnum;
	short e_shentsize;
	short e_shnum;
	short e_shstrndx;
};

struct SectionHeader
{
	SectionHeader(ifstream* file, int sectionHeaderStart, char* buff);
	int wordsh_name;
	int wordsh_type;
	int addrsh_addr;
	int offsh_offset;
	int wordsh_size;
	int wordsh_link;
	int wordsh_info;
	int wordsh_addralign;
	int wordsh_entsize;
};

struct ProgramHeader
{
	ProgramHeader(ifstream* file, int programHeaderStart, char* buff);
	int p_type;
	int p_offset;
	int p_vaddr;
	int p_padder;
	int p_filesz;
	int p_memsz;
	int p_flags;
	int p_align;
};

#endif 

