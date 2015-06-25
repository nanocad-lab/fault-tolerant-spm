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

	unsigned short e_type;
	unsigned short e_machine;
	unsigned int e_version;
	unsigned int e_entry; //memory address of the entry point from where the process starts executing
	unsigned int e_phoff; //points to start of program header table
	unsigned int e_shoff; //points to start of section header table
	unsigned int e_flags;
	unsigned short e_ehsize;
	unsigned short e_phentsize; //size of program header entry
	unsigned short e_phnum; //number of entries in program header table
	unsigned short e_shentsize; //size of section header table entry
	unsigned short e_shnum; //number of entries in section header table
	unsigned short e_shstrndx;
};

struct SectionHeader
{
	SectionHeader(ifstream* file, int sectionHeaderStart, char* buff);
	unsigned int wordsh_name;
	unsigned int wordsh_type;
	unsigned int addrsh_addr;
	unsigned int offsh_offset;
	unsigned int wordsh_size;
	unsigned int wordsh_link;
	unsigned int wordsh_info;
	unsigned int wordsh_addralign;
	unsigned int wordsh_entsize;
};

struct ProgramHeader
{
	ProgramHeader(ifstream* file, int programHeaderStart, char* buff);
	unsigned int p_type;
	unsigned int p_offset;
	unsigned int p_vaddr;
	unsigned int p_padder;
	unsigned int p_filesz;
	unsigned int p_memsz;
	unsigned int p_flags;
	unsigned int p_align;
};

#endif 

