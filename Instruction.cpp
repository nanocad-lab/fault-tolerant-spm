#include "Instruction.h"
#include "MiscFuncs.h"
#include <iostream>


Instruction::Instruction(std::string commandType, char instruction[], int instructionSizeInBytes)
: m_type(commandType), m_size(instructionSizeInBytes)
{
	m_numeric_instruction = 0;

	for (int i = 0; i < m_size; i++)
		m_instruction[i] = instruction[i];

	m_numeric_instruction = stringToNumericInstruction(instruction, instructionSizeInBytes);
}

int Instruction::getInstruction(char* buff) {
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

std::string typeOfInstruction(int instruction, int size)
{
	
	if (size == 2) //16-bit instruction
	{
		if ((instruction >> 12) == 0XD) //conditional
		{
			return "branch16conditional";
		}
		else if ((instruction >> 11) == 0X1C) //unconditional
		{
			return "branch16unconditional";
		}
		else if ((instruction &  0XB500)== 0XB100) //? check this
		{
			return "CBZ,CBNZ";
		}
		else
		{
			return "other";
		}
	}
	else if (size == 4) //32-bit instruction
	{
		
		if ((((instruction >> 27) & 0X1F) == 0X1E) && ((instruction & 0XC000) == 0X8000) && ((instruction& 0x1000) >> 12) == 0)
			return "branch32conditional";
		
		if ((((instruction >> 27) & 0X1F) == 0X1E) && ((instruction & 0XC000) == 0X8000) && ((instruction & 0x1000) >> 12) == 1)
			return "branch32unconditional";

		if ((((instruction >> 27) & 0X1F) == 0X1E) && ((instruction & 0XC000) == 0XC000) && ((instruction & 0x1000) >> 12) == 1)
			return "branch32L";


		return "32 bit not done yet";
	}
	else
	{
		std::cout << "Invalid command length given to typeOfInstruction" << std::endl;
		return "error";
	}
}


