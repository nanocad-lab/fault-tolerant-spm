#ifndef INSTRUCTION_INCLUDED
#define INSTRUCTION_INCLUDED

#include <string>

class Instruction
{
public:
	Instruction(unsigned int instruction, int instructionSizeInBytes, unsigned int addressInMemory); //creates a data structure holding the instruction
	int size() { return m_size;} //returns size in bytes
	std::string type() { return m_type; } //function has no use at the moment
	unsigned int getInstruction(char* buff); //function has no use at the moment
	unsigned int getNumericInstruction(){ return m_numeric_instruction; } //returns the 32 or 16-bit (zero-padded) representation of the the instruction
	void updateInstructions(int newinstruction); //function not used in current source, will be useful later

private:
	std::string m_type;
	char m_instruction [4]; //not filled in during creation; may not be too useful since the "numeric" version is already being stored
	int m_numeric_instruction;
	int m_size;
	int m_address;
	unsigned int m_opcode;
	Instruction* referencedInstruction;
};


/* stringToNumericInstruction converts a hex string representation of an instruction into numeric binary format
*/
unsigned int stringToNumericInstruction(char* command, int sizeInBytes, char endianness = 1); //assume little endian



/*  typeOfInstruction not implemented right now, perhaps categorizing instruction type by string isn't the most useful thing to do.
	can't really store into m_opcode either.
*/
std::string typeOfInstruction(int instruction, int sizeInBytes);


#endif 