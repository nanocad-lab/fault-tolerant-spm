#include "Instruction.h"
#include "MiscFuncs.h"
#include "ELF.h"
#include <vector>
#include <string>
#include <unordered_set>
#include <set>
#include <unordered_map>
#include <fstream> //for addresses.hex - note: if you are only reading, why use fstream instead of ifstream?
#include <iostream>
using namespace std;

int main()
{
	/*TODO: ADJUST WHERE LOOPS START AND END TO ONLY FIND ADDRESSES IN THE CODE SEGMENT.
	Elf part of the code was just written recently by Muzammil to figure out where code segment is.
	
	Initial assumption was that there is only code in the file, so the iterator in for loops such as the eip
	is not the eip of the code but the location of the char in the input file.*/


	//ADDRESS PARSING SECTION

	//load invalid addresses into unordered set
	unordered_set<unsigned int> badAddresses;

	cout << "Your working directory is: ";
	system("cd");
	cout << endl;

	FILE* addressFile;
	addressFile = fopen("addresses.txt", "r");
	if (addressFile == NULL) {
		cout << "Error: addresses.txt does not exist in the working directory or cannot be opened" << endl;
		return -1;
	}
	cout << "addresses.txt has been opened" << endl;
	char* addressBuff = new char[8];
	cout << "Loading addresses" << endl;
	while (!feof(addressFile)){ //after reading the final address, why does it read it again?
		int scanVal = fscanf(addressFile, "0x%8c ", addressBuff);
		string addressString(addressBuff);
		unsigned int intAddress = (unsigned int) stoul(addressString, nullptr, 16);
		badAddresses.insert(intAddress);

		/*for (int i = 0; i < 8; i++){
			cout << addressBuff[i];
		}
		cout << " = " << intAddress << endl;
		*/
	}

	cout << "Addresses successfully loaded" << endl;
	cout << "There are " << badAddresses.size() << " bad addresses" << endl;

	//clean up address parsing
	delete[] addressBuff;
	fclose(addressFile);
	cout << "addresses.txt has been closed\n" << endl;;


	//INSTRUCTION PARSING SECTION

	int input_eip = 0;
	int actual_eip = 0;
	char* currCommand = new char[5]; //arbitrary 5? or what
	int numCommand = 0; //int form of currCommand
	int lengthCommand = 0; //length of the current command

	string commandType;
	unordered_map<int, int> indexMap; // index of input, input_eip
	unordered_map<int, int>	outputindexMap; //  index of output, actual_eip
	unordered_map<int, int> addressMap; // input_eip, actual_eip
	unordered_map<int, int> reverseaddressMap; // actual_eip, input_eip
	vector<Instruction> input;
	vector<Instruction> output;
	char zeroes[4] = { 0, 0, 0, 0};
	Instruction space("space", zeroes, 16);
	char endianness;

	vector<ProgramHeader> programHeaderTable;
	vector<SectionHeader> sectionHeaderTable;

	unsigned int beg_code_segment;
	unsigned int end_code_segment;

	//load values for ELF variables, load all section headers and program headers
	ifstream fileset("program.elf", ios::in | ios::binary | ios::ate);
	if (!fileset){
		cout << "Error: program.elf does not exist in the working directory or cannot be opened" << endl;
		return -1;
	}
	else {
		cout << "program.elf has been opened" << endl;
		ElfHeader elfHeader(&fileset, 0, currCommand);
		endianness = elfHeader.e_ident[5];

		beg_code_segment = elfHeader.e_entry;
		cout << showbase;
		cout << "entry point address: " << hex << beg_code_segment << endl;

		//create program header and fill it in
		unsigned int progHdrReadAddr = elfHeader.e_phoff;
		cout << "Program header table starts at " << hex << progHdrReadAddr << endl;
		cout << "Program header size is " << dec << elfHeader.e_phentsize << " bytes" << endl;
		cout << "Program header number is " << dec << elfHeader.e_phnum << " headers" << endl;
		if (progHdrReadAddr != 0) { //if e_phnum is 0, then e_phoffm must also be 0
			for (int i = 0; i < elfHeader.e_phnum; i++)
			{
				ProgramHeader programHeader(&fileset, progHdrReadAddr, currCommand, endianness);
				programHeaderTable.push_back(programHeader);
				progHdrReadAddr += elfHeader.e_phentsize;
			}
			end_code_segment = beg_code_segment + programHeaderTable.at(0).p_memsz;
			cout << "Program headers loaded" << endl;
			cout << "There are " << dec << programHeaderTable.size() << " entries in the program header table" << endl;
		}
		else
			cout << "Warning: No program header table exists" << endl;

		//create section header and fill it in
		unsigned int sectReadAddr = elfHeader.e_shoff;
		cout << "Section header table starts at " << hex << sectReadAddr << endl;
		cout << "Section header size is " << dec << elfHeader.e_shentsize << " bytes" << endl;
		cout << "Section header number is " << dec << elfHeader.e_shnum << " headers" << endl;
		if (sectReadAddr != 0) { //if e_shnum is 0, then e_shoff must also be 0
			for (int i = 0; i < elfHeader.e_shnum; i++)
			{
				SectionHeader section(&fileset, sectReadAddr, currCommand, endianness);
				sectionHeaderTable.push_back(section);
				sectReadAddr += elfHeader.e_shentsize;
			}
			cout << "Section headers loaded" << endl;
			cout << "There are " << dec << sectionHeaderTable.size() << " entries in the section header table" << endl;
		}
		else
			cout << "Warning: No section header table exists" << endl;
		fileset.close();
		cout << "program.elf has been closed\n" << endl;
	}

	//SORT THROUGH FILE AND ADJUST CODE 
	//needs optimization in future after gotten working
	ifstream file("program.elf", ios::in | ios::binary | ios::ate);
	if (!file){
		cout << "Error: program.elf does not exist in the working directory or cannot be opened" << endl;
		return -1;
	}
	else {
		cout << "program.elf has been opened" << endl;

		//NOTE TO SELF: NOW BEGIN SIFTING THROUGH CODE SEGMENT NOW THAT LIMITS HAVE BEEN FOUND

		// load instrucVec with all commands
		input_eip = beg_code_segment;
		int offset = 0;
		int index = 0;
		while ((beg_code_segment + offset) < end_code_segment){
			int new_offset = offset;
			file.seekg(beg_code_segment + offset);
			file.read(currCommand, 2);

			numCommand = stringToIntInstruction(currCommand, 2, endianness);
			
			if (is32Bit(numCommand)){
				lengthCommand = 4;
				new_offset += 4;
				file.read(currCommand + 2, 2);
				changeEndian(currCommand + 2, 2);
				numCommand = numCommand << 16;
				numCommand |= stringToIntInstruction(currCommand + 2, 2, endianness);
			}
			else{
				lengthCommand = 2;
				new_offset += 2;
			}
			string stringCommand = int_to_hexString(numCommand, lengthCommand);
			//create and push instrucs into tempvec
			commandType = typeOfInstruction(numCommand, lengthCommand);

			Instruction currInstruc(commandType, currCommand, lengthCommand);
			input.push_back(currInstruc);
			indexMap.insert(make_pair(index, input_eipstarting)); // index, input_eip

			index++;
			offset = new_offset;
		}
		/*for (int index = 0; input_eip < end_code_segment; index++) //iterate 16bits at a time  index is for matching index and input_eip in indexMap
		{
			if (endianness == 2) {
				cout << "Error: Can't handle big endian right now!" << endl;
				return -1;
			}

			//current code inside this loop assumes little endian
			file.seekg(input_eip);
			file.read(currCommand, 2); //read first two bytes
			int input_eipstarting = input_eip; // location of current instruction

			numCommand = stringToIntInstruction(currCommand, 2, endianness);

			if (is32Bit(numCommand)) {//if not true then 32bit instruction, else 16 bit -- checking to see if op1 == 00 (binary)
				lengthCommand = 32;
				input_eip += 4;
				file.read(currCommand + 2, 2); //read next 16 bits  
				//currCommand[4] = '\0';
				changeEndian(currCommand + 2, sizeof(currCommand)/sizeof(currCommand[0])); // convert next 16 bits, doesn't work correctly right now
				numCommand = numCommand << 16;
				numCommand += stringToIntInstruction(currCommand + 2, sizeof(currCommand)/sizeof(currCommand[0]), '1'); //add on next 16 bits
			}
			else
			{
				lengthCommand = 16;
				input_eip += 2;
			}

			string stringcommand = int_to_hexString(numCommand, lengthCommand / 4);

			//create and push instrucs into tempvec
			commandType = typeOfInstruction(numCommand, lengthCommand);
			
			Instruction currInstruc(commandType, currCommand, lengthCommand);
			input.push_back(currInstruc);
			indexMap.insert(make_pair(index, input_eipstarting)); // index, input_eip
		}*/



		//reset loop and go through loop again
		// insert extra branches and space
		// adjust offsets
		for (int i = 0, input_eip = 0; i < input.size();)
		{
			
			int instrucLength = input[i].length();
			input_eip += instrucLength / 8;
			int startingeip = actual_eip;
			int tempVectorIndex = i;
			int bytesReplaced = 0;
			while (invalidAddressDetected(badAddresses, actual_eip, 2 * bytesReplaced + 4))
			{
				bytesReplaced += 8; //block out 32 bits if invalid

				actual_eip += 8; // this space will be taken by the space instruction (bytesReplaced)
				tempVectorIndex++;
			}

			if (bytesReplaced > 0)
			{
				//check what branch length to use
				//add bit zero check and set later

				//if 16bit branch is required assuming 16 bit instruction is next
				int imm = (actual_eip - startingeip - 4) >> 1; 
				int encoding = 0XE000 | imm;
				char temp[2];
				hexToCharArr(temp, encoding, 2);
				changeEndian(temp, sizeof(temp)/sizeof(temp[0]));
				Instruction branch("insertedbranch", temp, 16);
				output.push_back(branch);
				actual_eip += 2;

				//if 32bit branch is required

				//actual eip for this temp already taken care of in above while loop checking for invalid addresses

				for (int a = 0; a < bytesReplaced / 4; a += 1)
				{
					output.push_back(space);
				}

				for (int j = 0; j < bytesReplaced;)
				{
					output.push_back(input[i]);
					//find out input eip
					input_eip = indexMap.find(i)->second;   //should add a safety check here
					//insert input_eip and actual pair to addressmap
					addressMap.insert(make_pair(input_eip, actual_eip));
					reverseaddressMap.insert(make_pair(actual_eip, input_eip));
					outputindexMap.insert(make_pair(output.size()-1, actual_eip)); //index, actual_eip
					actual_eip += input[i].length() / 8;
					j += input[i].length() / 8;
					i++;
				}
				bytesReplaced = 0;
			}
			else
			{
				//no problems push to output and increment 
				output.push_back(input[i]);
				//find out input eip
				input_eip = indexMap.find(i)->second;   //should add a safety check here
				//insert input_eip and actual pair to addressmap
				addressMap.insert(make_pair(input_eip, actual_eip));
				reverseaddressMap.insert(make_pair(actual_eip, input_eip));
				outputindexMap.insert(make_pair(output.size() - 1, actual_eip)); //index, actual_eip
				actual_eip += input[i].length()/8;
				i++;
			}
		}

		//adjust branch offsets
		int limit = output.size();
		for (int n = 0; n < limit; n++)
		{

			if (output[n].type().find("branch16conditional") != string::npos)
			{
				int oldinstruction = output[n].giveInstructionInt();
				int tempImm = signExtend32((oldinstruction & 0XFF), 8); 
				tempImm = (tempImm ) << 1;
				int curraddress = outputindexMap.find(n)->second; // gives address of this in output file   
				int inputaddress = reverseaddressMap.find(curraddress)->second; // address of branch instruc in input file
				//what f going to 32 bit instruc
				int targetaddress = inputaddress + tempImm + 4;  
							
				if (addressMap.find(targetaddress) == addressMap.end()) //checking to see if branch is trying to branch location not in this file
				{
					cout << "problemo" << endl;
					continue;
				}
				int newtargetaddress = addressMap.find(targetaddress)->second; //gives address of this in output
				int updatedimm = (newtargetaddress - curraddress - 4); // is 2 pc ?
				updatedimm = updatedimm >> 1; 
				//UPDATE INSTRUC DATAMEMEBERS
				int updatedinstruction = (oldinstruction & 0XFF00) | (updatedimm & 0XFF);
				output[n].updateInstructions(updatedinstruction);
				
			}
			else if (output[n].type().find("branch16unconditional") != string::npos)
			{
				int oldinstruction = output[n].giveInstructionInt();
				int tempImm = signExtend32((oldinstruction & 0X7FF), 11); 
				tempImm = tempImm << 1;
				int curraddress = outputindexMap.find(n)->second; // gives address of this in output file   
				int inputaddress = reverseaddressMap.find(curraddress)->second; // address of branch instruc in input file
				int targetaddress = inputaddress + tempImm + 4;  
				
				if (addressMap.find(targetaddress) == addressMap.end()) //checking to see if branch is trying to branch location not in this file
				{
					cout << "problemo" << endl;
					continue;
				}
				int newtargetaddress = addressMap.find(targetaddress)->second; //gives address of this in output
				int updatedimm = (newtargetaddress - curraddress - 4); 
				updatedimm = updatedimm >> 1;
				//UPDATE INSTRUC DATAMEMEBERS
				int updatedinstruction = (oldinstruction & 0XF800) | (updatedimm & 0X7FF);
				output[n].updateInstructions(updatedinstruction);
			}
			else if (output[n].type().find("branch32conditional") != string::npos) 
			{
				int oldinstruction = output[n].giveInstructionInt();
				int S = (oldinstruction & 0X4000000) >> 26;
				int J1 = (oldinstruction & 0X2000) >> 13;
				int J2 = (oldinstruction & 0x800) >> 11;

				int imm6 = (oldinstruction >> 16) & 0X3F;
				int imm11 = oldinstruction & 0X7FF;
				int tempImm = (S<<19) |(J2 << 18) | (J1 << 17) | (imm6 << 11) | imm11;
				tempImm = signExtend32(tempImm, 20);
				tempImm = tempImm << 1;
				int curraddress = outputindexMap.find(n)->second; // gives address of this in output file   
				int inputaddress = reverseaddressMap.find(curraddress)->second; // address of branch instruc in input file
				int targetaddress = inputaddress + tempImm + 4;

				if (addressMap.find(targetaddress) == addressMap.end()) //checking to see if branch is trying to branch location not in this file
				{
					cout << "problemo" << endl;
					continue;
				}
				int newtargetaddress = addressMap.find(targetaddress)->second; //gives address of this in output
				int updatedimm = (newtargetaddress - curraddress - 4);
				updatedimm = updatedimm >> 1;

				S = ((updatedimm  & 0X80000) >> 19) & 0X1;
				J1 = ((updatedimm & 0X20000) >> 17) & 0X1;
				J2 = ((updatedimm & 0x40000) >> 18) & 0X1;
				imm6 = updatedimm & 0X1F800;
				imm11 = updatedimm & 0X7FF;

				//UPDATE INSTRUC DATAMEMEBERS
				int updatedinstruction = (oldinstruction & 0XFBC0D000) | (S << 26) | (J1 << 13) | (J2 << 11) | (imm6 << 5) | imm11;
				output[n].updateInstructions(updatedinstruction);
				
				
			}
			else if (output[n].type().find("branch32unconditional") != string::npos)
			{
				int oldinstruction = output[n].giveInstructionInt();
				int S = (oldinstruction & 0X4000000) >> 26;
				int J1 = (oldinstruction & 0X2000) >> 13;
				int J2 = (oldinstruction & 0x800) >> 11;
				int I1 = (~(J1 ^ S)) & 0x1;
				int I2 = (~(J2 ^ S)) & 0x1;
				int imm10 = (oldinstruction >> 16) & 0X3FF;
				int imm11 = oldinstruction & 0X7FF;
				int tempImm = (S << 23) |(I1 << 22) | (I2 << 21) | (imm10 << 11) | imm11;
				tempImm = signExtend32(tempImm, 24);
				tempImm = tempImm << 1;
				int curraddress = outputindexMap.find(n)->second; // gives address of this in output file   
				int inputaddress = reverseaddressMap.find(curraddress)->second; // address of branch instruc in input file
				int targetaddress = inputaddress + tempImm + 4;

				if (addressMap.find(targetaddress) == addressMap.end()) //checking to see if branch is trying to branch location not in this file
				{
					cout << "problemo" << endl;
					continue;
				}
				int newtargetaddress = addressMap.find(targetaddress)->second; //gives address of this in output
				int updatedimm = (newtargetaddress - curraddress - 4);
				updatedimm = updatedimm >> 1;

				S = ((updatedimm & 0X800000) >> 23) & 0X1;
				I1 = ((updatedimm & 0X400000) >> 22) & 0X1;
				I2 = ((updatedimm & 0x200000) >> 21) & 0X1;
				J1 = (~I1 ^ S) & 0X1;
				J2 = (~I2 ^ S) & 0X1;

				//UPDATE INSTRUC DATAMEMEBERS
				int updatedinstruction = (oldinstruction & 0XFBC0D000) | (S << 26) | (J1 << 13) | (J2 << 11) | ((updatedimm & 0X1FF800) << 5) | (updatedimm & 0X7FF);
				//test
				if (updatedinstruction != oldinstruction) //needs additional checks
					cout << "garbanzo" << endl;
				output[n].updateInstructions(updatedinstruction);
			}
			else if (output[n].type().find("branch32L") != string::npos) // seems to have same offset as b-unconditional
			{
				int oldinstruction = output[n].giveInstructionInt();
				int S = (oldinstruction & 0X4000000) >> 26;
				int J1 = (oldinstruction & 0X2000) >> 13;
				int J2 = (oldinstruction & 0x800) >> 11;
				int I1 = (~(J1 ^ S)) & 0x1;
				int I2 = (~(J2 ^ S)) & 0x1;
				int imm10 = (oldinstruction >> 16) & 0X3FF;
				int imm11 = oldinstruction & 0X7FF;
				int tempImm = (S << 23) | (I1 << 22) | (I2 << 21) | (imm10 << 11) | imm11;
				tempImm = signExtend32(tempImm, 24);
				tempImm = tempImm << 1;
				int curraddress = outputindexMap.find(n)->second; // gives address of this in output file   
				int inputaddress = reverseaddressMap.find(curraddress)->second; // address of branch instruc in input file
				int targetaddress = inputaddress + tempImm + 4;

				if (addressMap.find(targetaddress) == addressMap.end()) //checking to see if branch is trying to branch location not in this file
				{
					cout << "problemo" << endl;
					continue;
				}
				int newtargetaddress = addressMap.find(targetaddress)->second; //gives address of this in output
				int updatedimm = (newtargetaddress - curraddress - 4);
				updatedimm = updatedimm >> 1;

				S = ((updatedimm & 0X800000) >> 23) & 0X1;
				I1 = ((updatedimm & 0X400000) >> 22) & 0X1;
				I2 = ((updatedimm & 0x200000) >> 21) & 0X1;
				J1 = (~I1 ^ S) & 0X1;
				J2 = (~I2 ^ S) & 0X1;

				//UPDATE INSTRUC DATAMEMEBERS
				int updatedinstruction = (oldinstruction & 0XFBC0D000) | (S << 26) | (J1 << 13) | (J2 << 11) | ((updatedimm & 0X1FF800) << 5) | (updatedimm & 0X7FF);
				output[n].updateInstructions(updatedinstruction);
			}
			else
			{
			}
		}


		//output results to file
		ofstream outfile("newprogram.elf", ofstream::binary);
		cout << "newprogram.elf is being written to" << endl;
		char* buff = new char[4];
		//write to outputfile
		int lineCount = 1;
		for (int k = 0; k < output.size(); k++)   
		{
			output[k].giveInstruction(buff);
			changeEndian(buff, sizeof(buff)/sizeof(buff[0]));

			if (output[k].length() == 16)
			{
				outfile.write(buff, 2);
			}
			else
			{
				outfile.write(buff, 2);
				outfile.write(buff + 2, 2);
			}
		}
		//clean up
		delete[] buff;
		outfile.close();
		cout << "newprogram.elf has been closed" << endl;
	}

	//clean up
	delete[] currCommand;
	file.close();
	cout << "program.elf has been closed\n" << endl;

	cout << "Done" << endl;

}
