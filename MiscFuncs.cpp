//C libraries
#include <unordered_set>
#include <iostream>
#include <iomanip>
#include <sstream>

//user defined
#include "MiscFuncs.h"
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

void changeEndian(char* buff, int buffLength) //switch buffer's Endianness
{
	for (int i = 0; i < buffLength / 2; i++){
		int temp = buff[buffLength - 1 - i];
		buff[buffLength - 1 - i] = buff[i];
		buff[i] = temp;
	}
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

bool is32Bit(int begcommand) //if bits [15:11] of half-word being decoded are 0b11101, 0b11110, or 0b11111, it's first half-word of 32-bit instruction
{
	if ((begcommand & 0xE000) != 0xE000) //if bits [15:13] aren't 1s, not 32-bit
		return false;

	if ((begcommand & 0xF800) == 0xE000) //if bits [15:13] are 1s but [12:11] are 0s, not 32-bit
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
