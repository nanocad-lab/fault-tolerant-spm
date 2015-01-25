#ifndef MISCFUNCS_INCLUDED
#define MISCFUNCS_INCLUDED

#include <unordered_set>
#include <iostream>
#include <iomanip>
#include <sstream>
using namespace std;

// check to see if the address is safe to write
// if next or current command is at an invalid address will return true
// signal to insert jumps
bool invalidAddressDetected(unordered_set<int>& set, int addressStart, int continueLength)
{
	for (int i = addressStart; i < (addressStart + 16 + continueLength); i++){
		if (set.find(i) != set.end()){
			return true;
		}
	}

	return false;
}

void changeEndian(char* command, int length = 16, int version = 1) //convert 16 or 32 bit command to Big Endian
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

			temp1 = command[0];
			command[0] = command[2];
			command[2] = temp1;

			temp1 = command[1];
			command[1] = command[3];
			command[3] = temp1;
			

		}
		else if (length == 32)
		{
			temp1 = command[0];
			command[0] = command[2];
			command[2] = temp1;

			temp1 = command[1];
			command[1] = command[3];
			command[3] = temp1;

			temp1 = command[4];
			command[4] = command[6];
			command[6] = temp1;

			temp1 = command[5];
			command[5] = command[7];
			command[7] = temp1;  
		}
		else
			std::cout << "INVALID LENGTH ADDRESS - NO CONVERSION";
	}

	return;
}

// modified from http://stackoverflow.com/questions/5100718/int-to-hex-string-in-c
template< typename T >
string int_to_hexString(T i, int filler)
{
	stringstream stream;

	stream << setfill('0') << setw(filler) << hex << i;
	return stream.str();
}

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

bool is32Bit(int begcommand)
{
	int test = begcommand & 0xF800;
	if (((test >> 13) << 1) != 0xE)
		return false;
	if ((begcommand & 0x1800) == 0)
		return false;
	return true;
}

#endif