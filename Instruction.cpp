#include "Instruction.h"
#include <iostream>


Instruction::Instruction(std::string commandType, std::string instruction, int size)
: m_type(commandType), m_size(size)
{
	m_instruction = instruction.substr(0,size/4);
	
}

int stringToIntInstruction(char* command, int length) // fix this
{
	
	length = length / 8;
	int result = 0;
	for (int i = 0; i < length; i++)
	{	
		result = result << (8 * i);
		result += *(command + i ) & 0XFF;
		
	}
	return result;
}

std::string typeOfInstruction(int instruction, int commandLength)
{
	if (commandLength == 16)
	{
		switch ((instruction & 0XFC00) >> 10)
		{
		case 34: //conditional
		case 35:
		case 36:
		case 37:
			if (((instruction & 0XF00) != 0XE) && ((instruction & 0XF00) != 0XF))
			{
				return "branch";
			}
			else
				return "other";
		case 38: //unconditional
		case 39:
			return "branch";
		default:
			return "other";
		};
	}
	else if (commandLength == 32)
	{
		return "not done yet";
	}
	else
	{
		std::cout << "Invalid command length given to typeOfInstruction" << std::endl;
		return "error";
	}
}


