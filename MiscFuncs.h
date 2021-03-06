#ifndef MISCFUNCS_INCLUDED
#define MISCFUNCS_INCLUDED

#include <unordered_set>
#include <iomanip>
#include <sstream>
using namespace std;


// check to see if the address is safe to write
// if next or current command is at an invalid address will return true
// signal to insert jumps

bool invalidAddressDetected(unordered_set<unsigned int>& set, int addressStart, int continueLength);


void changeEndian(char* command, int buffLength);



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
