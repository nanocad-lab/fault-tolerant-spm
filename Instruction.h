#ifndef INSTRUCTION_INCLUDED
#define INSTRUCTION_INCLUDED

#include <string>

class Instruction
{
public:
	Instruction(unsigned int instruction, int instructionSizeInBytes, unsigned int addressInMemory);
	int size() { return m_size;}
	std::string type() { return m_type; }
	unsigned int getInstruction(char* buff);
	unsigned int getNumericInstruction(){ return m_numeric_instruction; }
	void updateInstructions(int newinstruction);
private:
	std::string m_type;
	char m_instruction [4];
	int m_numeric_instruction;
	int m_size;
	int m_address;
	unsigned int m_opcode;
};

unsigned int stringToNumericInstruction(char* command, int sizeInBytes, char endianness = 1); //assume little endian

std::string typeOfInstruction(int instruction, int commandLengthInBytes);







#endif 