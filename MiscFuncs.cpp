#include "MiscFuncs.h"
#include <unordered_set>
#include <iostream>
#include <iomanip>
#include <sstream>
using namespace std;

// check to see if the address is safe to write
// if next or current command is at an invalid address will return true
// signal to insert jumps
bool invalidAddressDetected(unordered_set<unsigned int>& set, int addressStart, int continueLength)
{
	for (int i = addressStart; i < (addressStart + continueLength); i++){
		if (set.find(i) != set.end()){
			return true;
		}
	}

	return false;
}

void changeEndian(char* command, int length , int version) //convert 16 or 32 bit command to Big Endian
{
	char temp1;
	if (version == 1)
	{
		if (length == 16)   //is this one necessary?
		{
			temp1 = command[0];
			command[0] = command[1];
			command[1] = temp1;


		}
		else if (length == 32)
		{
			temp1 = command[0];
			command[0] = command[1];
			command[1] = temp1;

			temp1 = command[2];
			command[2] = command[3];
			command[3] = temp1;
		}
		else
			std::cout << "INVALID LENGTH ADDRESS - NO CONVERSION";
	}
	else
	{
		if (length == 16)   //is this one necessary?
		{

		


		}
		else if (length == 32)
		{
			temp1 = command[0];
			command[0] = command[3];
			command[3] = temp1;

			temp1 = command[1];
			command[1] = command[2];
			command[2] = temp1;

			/*temp1 = command[4];
			command[4] = command[6];
			command[6] = temp1;

			temp1 = command[5];
			command[5] = command[7];
			command[7] = temp1;*/
		}
		else
			std::cout << "INVALID LENGTH ADDRESS - NO CONVERSION";
	}

	return;
}


//Was planning on using this to cleanup source but haven't done that yet
void adjustBranch(int instruction, int location, int target, int length)
{
	if (length == 16)
	{
		int imm = instruction & 0X7FF;
		imm = imm << 2;
		//instruction & 0X8000;

	}
	else if (length == 32)
	{
	}
	else
	{
		cout << "error" << endl;
	}
	return;

}

bool is32Bit(int begcommand) //is this a 32 bit command
{
	if ((begcommand & 0XE000) != 0XE000)
		return false;

	if ((begcommand & 0XF800) == 0XE000)
		return false;


	return true;
}

void hexToCharArr(char* buff, int num, int bytes) //fill buff with char version of the input number
{
	int mask = 0xFF;
	for (int i = 0; i < bytes; i += 1)
	{
		buff[i] = num&mask;
		num = num >> 8;
	}
	return;
}

int signExtend32(int instr, int immlength) { //sign extend the input to 32 bit given the length of the inout
	int value = instr;
	int lengthmask = 0;
	for (int i = 0; i < immlength; i++)
	{
		lengthmask = lengthmask << 1;
		lengthmask = lengthmask | 1;
	}
	if (immlength <= 0)
		return value;
	value = value & lengthmask;
	int mask = 1 << (immlength - 1);
	if (mask & instr) {
		value = value | (~lengthmask);
	}
	return value;
}