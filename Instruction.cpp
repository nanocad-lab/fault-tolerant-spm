#include "Instruction.h"
#include "MiscFuncs.h"
#include <iostream>


Instruction::Instruction(unsigned int instruction, int instructionSizeInBytes, unsigned int addressInMemory)
	: m_numeric_instruction(instruction), m_size(instructionSizeInBytes), m_address(addressInMemory)
{
	referencedInstruction = nullptr;
	//m_opcode = typeOfInstruction(instruction, instructionSizeInBytes)?
}

unsigned int Instruction::getInstruction(char* buff) {
	if (buff != nullptr)
	{
		for (int i = 0; i < m_size / 8; i++)
		{
			buff[i] = m_instruction[i];
		}
		return m_size;
	}
	return -1;
}

void Instruction::updateInstructions(int newinstruction) //incomplete
{
	m_numeric_instruction = newinstruction;
	int mask = 0xFF;
	for (int i = m_size-8; i >= 0; i -= 8)
	{
		m_instruction[i / 8] = (mask & newinstruction);
		newinstruction = newinstruction >> 8;
	}
	return;
}

unsigned int stringToNumericInstruction(char* command, int sizeInBytes, char endianness)
{	
	if (endianness == 1)//1 corresponds to little endian, 2 corresponds to big endian
		changeEndian(command, sizeInBytes);
	unsigned int result = 0;
	for (int i = 0; i < sizeInBytes; i++)
	{	
		result = result << (8);
		result |= (command[i] & 0xFF);
	}
	return result;
}

std::string typeOfInstruction(int instruction, int sizeInBytes)
{
	//not working, check by opcode
	return "NOT WORKING";
}


