#include "ELF.h"
#include "MiscFuncs.h"
#include "Instruction.h"

SectionHeader::SectionHeader(ifstream* file, int headerStart, char* buff)
{
	//int name, int type, int addr, int offset, int size, int link, int info, int addralign, int entsize
	file->seekg(headerStart);
	file->read(buff, 4);
	changeEndian(buff, 32, 2);
	wordsh_name = stringToIntInstruction(buff, 32);

	file->seekg(headerStart+4);
	file->read(buff, 4);
	changeEndian(buff, 32, 2);
	wordsh_type = stringToIntInstruction(buff, 32);

	file->seekg(headerStart+8);
	file->read(buff, 4);
	changeEndian(buff, 32, 2);
	addrsh_addr = stringToIntInstruction(buff, 32);

	file->seekg(headerStart+12);
	file->read(buff, 4);
	changeEndian(buff, 32, 2);
	offsh_offset = stringToIntInstruction(buff, 32);

	file->seekg(headerStart+16);
	file->read(buff, 4);
	changeEndian(buff, 32, 2);
	wordsh_size = stringToIntInstruction(buff, 32);

	file->seekg(headerStart+20);
	file->read(buff, 4);
	changeEndian(buff, 32, 2);
	wordsh_link = stringToIntInstruction(buff, 32);

	file->seekg(headerStart+24);
	file->read(buff, 4);
	changeEndian(buff, 32, 2);
	wordsh_info = stringToIntInstruction(buff, 32);

	file->seekg(headerStart+28);
	file->read(buff, 4);
	changeEndian(buff, 32, 2);
	wordsh_addralign = stringToIntInstruction(buff, 32);

	file->seekg(headerStart+32);
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