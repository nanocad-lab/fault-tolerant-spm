#ifndef MISCFUNCS_INCLUDED
#define MISCFUNCS_INCLUDED

#include <unordered_set>
#include <iomanip>
#include <sstream>
//#include <iostream>
//#include <iomanip>
//#include <sstream>
using namespace std;


// check to see if the address is safe to write
// if next or current command is at an invalid address will return true
// signal to insert jumps

bool invalidAddressDetected(unordered_set<int>& set, int addressStart, int continueLength);


//version 1: swap order of every 8 bytes
//version 2: swap order of entire thing
void changeEndian(char* command, int length = 16, int version = 1);



// modified from http://stackoverflow.com/questions/5100718/int-to-hex-string-in-c
template< typename T >
string int_to_hexString(T i, int filler)
{
	stringstream stream;

	stream << setfill('0') << setw(filler) << hex << i;
	return stream.str();
}

void adjustBranch(int instruction, int location, int target, int length);

bool is32Bit(int begcommand);

void hexToCharArr(char* buff, int num, int bytes);

int signExtend32(int instr, int immlength);


#endif