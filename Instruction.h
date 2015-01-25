#ifndef INSTRUCTION_INCLUDED
#define INSTRUCTION_INCLUDED

#include <string>

class Instruction
{
public:
	Instruction(std::string commandType, std::string instruction, int size);
	int length() { return m_size;}
	std::string type() { return m_type; }
	std::string giveInstruction() {return m_instruction;}
	
private:
	std::string m_type;
	std::string m_instruction;
	int m_size;
};

int stringToIntInstruction(char* command, int length = 16);



std::string typeOfInstruction(int instruction, int commandLength);







#endif 