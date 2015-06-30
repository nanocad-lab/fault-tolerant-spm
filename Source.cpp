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

	int inputeip = 0;
	int actualeip = 0;
	char* currCommand = new char[5]; //arbitrary 5? or what
	int numCommand = 0; //int form of currCommand
	int lengthCommand = 0; //length of the current command

	string commandType;
	unordered_map<int, int>  indexMap; // index of input, inputeip
	unordered_map<int, int>	 outputindexMap; //  index of output, actualeip
	unordered_map<int, int> addressMap; // inputeip, actualeip
	unordered_map<int, int> reverseaddressMap; // actualeip, inputeip
	vector<Instruction> input;
	vector<Instruction> output;
	char zeroes[4] = { 0, 0, 0, 0};
	Instruction space("space", zeroes, 16);
	char endianness;

	vector<ProgramHeader> programTable;
	vector<SectionHeader> sectionTable;


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
		inputeip = elfHeader.e_entry; //NEW ADDITION, NEEDS TO BE TESTED

		//create program header and fill it in
		unsigned int progHdrReadAddr = elfHeader.e_phoff;
		if (progHdrReadAddr != 0) { //assumption: if e_phnum is 0, then e_phoff is necessarily 0; caught by if statement
			for (int i = 0; i < (elfHeader.e_phnum - 1); i++) //simple ARM executable, only three types of segment: Text, Data, BSS
			{
				ProgramHeader programHeader(&fileset, progHdrReadAddr, currCommand, endianness);
				programTable.push_back(programHeader);
				progHdrReadAddr += elfHeader.e_phentsize;
			}
			cout << "Program headers loaded" << endl;
		}
		else
			cout << "Warning: No program header table exists" << endl;

		//create section header and fill it in
		unsigned int sectReadAddr = elfHeader.e_shoff;
		if (sectReadAddr != 0) { //assumption: if e_shnum is 0, then e_phoff is 0
			for (int i = 0; i < (elfHeader.e_shnum - 1); i++)
			{
				SectionHeader section(&fileset, sectReadAddr, currCommand, endianness);
				sectionTable.push_back(section);
				sectReadAddr += elfHeader.e_shentsize;
			}
			cout << "Section headers loaded" << endl;
		}
		else
			cout << "Warning: No section header table exists" << endl;
		fileset.close();
		cout << "program.elf has been closed\n" << endl;
	}

	//needs optimization <- what needs optimization? -Albert

	// begin sorting through file again 
	ifstream file("program.elf", ios::in | ios::binary | ios::ate);
	if (!file){
		cout << "Error: program.elf does not exist in the working directory or cannot be opened" << endl;
		return -1;
	}
	else {
		cout << "program.elf has been opened" << endl;

		//find end of file
		file.seekg(0, file.end);
		int end = file.tellg();
		file.seekg(0, file.beg);

		// load instrucVec with all commands
		for (int index = 0; inputeip < end; index++) //iterate 16bits at a time  index is for matching index and inputeip in indexMap
		{
			file.seekg(inputeip);
			file.read(currCommand, 2); //read 16 bits
			int inputeipstarting = inputeip; // location of current instruction

			numCommand = stringToIntInstruction(currCommand, 2, endianness);

			if (is32Bit(numCommand)) {//if not true then 32bit instruction, else 16 bit -- checking to see if op1 == 00 (binary)
				lengthCommand = 32;
				inputeip += 4;
				file.read(currCommand + 2, 2); //read next 16 bits  
				//currCommand[4] = '\0';
				changeEndian(currCommand + 2, sizeof(currCommand)/sizeof(currCommand[0])); // convert next 16 bits
				numCommand = numCommand << 16;
				numCommand += stringToIntInstruction(currCommand + 2, sizeof(currCommand)/sizeof(currCommand[0]), '1'); //add on next 16 bits
			}
			else
			{
				lengthCommand = 16;
				inputeip += 2;
			}

			string stringcommand = int_to_hexString(numCommand, lengthCommand / 4);

			//create and push instrucs into tempvec
			commandType = typeOfInstruction(numCommand, lengthCommand);
			
			Instruction currInstruc(commandType, currCommand, lengthCommand);
			input.push_back(currInstruc);
			indexMap.insert(make_pair(index, inputeipstarting)); // index, inputeip
		}



		//reset loop and go through loop again
		// insert extra branches and space
		// adjust offsets
		for (int i = 0, inputeip = 0; i < input.size();)
		{
			
			int instrucLength = input[i].length();
			inputeip += instrucLength / 8;
			int startingeip = actualeip;
			int tempVectorIndex = i;
			int bytesReplaced = 0;
			while (invalidAddressDetected(badAddresses, actualeip, 2 * bytesReplaced + 4))
			{
				bytesReplaced += 8; //block out 32 bits if invalid

				actualeip += 8; // this space will be taken by the space instruction (bytesReplaced)
				tempVectorIndex++;
			}

			if (bytesReplaced > 0)
			{
				//check what branch length to use
				//add bit zero check and set later

				//if 16bit branch is required assuming 16 bit instruction is next
				int imm = (actualeip - startingeip - 4) >> 1; 
				int encoding = 0XE000 | imm;
				char temp[2];
				hexToCharArr(temp, encoding, 2);
				changeEndian(temp, sizeof(temp)/sizeof(temp[0]));
				Instruction branch("insertedbranch", temp, 16);
				output.push_back(branch);
				actualeip += 2;

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
					inputeip = indexMap.find(i)->second;   //should add a safety check here
					//insert inputeip and actual pair to addressmap
					addressMap.insert(make_pair(inputeip, actualeip));
					reverseaddressMap.insert(make_pair(actualeip, inputeip));
					outputindexMap.insert(make_pair(output.size()-1, actualeip)); //index, actualeip
					actualeip += input[i].length() / 8;
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
				inputeip = indexMap.find(i)->second;   //should add a safety check here
				//insert inputeip and actual pair to addressmap
				addressMap.insert(make_pair(inputeip, actualeip));
				reverseaddressMap.insert(make_pair(actualeip, inputeip));
				outputindexMap.insert(make_pair(output.size() - 1, actualeip)); //index, actualeip
				actualeip += input[i].length()/8;
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
