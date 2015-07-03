#ifndef INSTRUCTION_INCLUDED
#define INSTRUCTION_INCLUDED

#include <string>

class Instruction
{
public:
	Instruction(std::string commandType, char instruction [], int size);
	int length() { return m_size;}
	std::string type() { return m_type; }
	int giveInstruction(char* buff);
	int giveInstructionInt(){ return m_instruction_int; }
	void updateInstructions(int newinstruction);
private:
	std::string m_type;
	char m_instruction [4];
	int m_instruction_int;
	int m_size;
};

unsigned int stringToIntInstruction(char* command, int sizeInBytes, char endianness = 1); //assume little endian

std::string typeOfInstruction(int instruction, int commandLength);







#endif 