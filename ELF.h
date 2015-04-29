#ifndef ELF_INCLUDED
#define ELF_INCLUDED

#include <fstream>
//#include <iostream>
using namespace std;

class SectionHeader
{
public:
	SectionHeader(ifstream* file, int headerStart, char* buff);
private:
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

class ProgramHeader
{
public:
	ProgramHeader(ifstream* file, int programHeaderStart, char* buff);
private:
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

