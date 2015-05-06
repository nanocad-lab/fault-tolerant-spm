
#include "Instruction.h"
#include "MiscFuncs.h"
#include "ELF.h"
#include <vector>
#include <string>
#include <unordered_set>
#include <set>
#include <unordered_map>
#include <fstream>
#include <iostream>
using namespace std;




int main()
{


	//load invalid addresses into unordered set
	unordered_set<int> badAddresses;
	fstream addressFile("addresses.hex");
	if (addressFile)
	{
		char * address = new char[8]; //same number of elements as address size
		addressFile.seekg(0, addressFile.end);
		int end = addressFile.tellg();
		addressFile.seekg(0, addressFile.beg);
		cout << "Loading adresses" << endl;
		for (int i = 0; ((i + 9) <= end); i += 10)
		{
			addressFile.seekg(i);
			addressFile.read(address, 4);
			addressFile.seekg(i + 5);
			addressFile.read(address + 4, 4);
			string addressString(address);
			int test = stoi(addressString, nullptr, 16);
			
			badAddresses.insert(stoi(addressString, nullptr, 16));
		}
		cout << "There are " << badAddresses.size() << " bad addresses" << endl;
		delete[] address;
		addressFile.close();
	}



	//setup vars
	int inputeip = 0;
	int actualeip = 0;
	char * currCommand = new char[5];
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

	vector<ProgramHeader> programTable;
	vector<SectionHeader> sectionTable;

	int e_entry = 0;  //memory address of the entry point from where the process starts executing
	int e_phoff = 0; //points to start of program header table
	int e_shoff = 0; //points to start of section header table
	int e_phentsize = 0; //size of program header entry
	int e_phnum = 0; //number of entries in program header table
	int e_shentsize = 0; //size of section header table entry
	int e_shnum = 0; //number of entries in section header table

	//load values for ELF variables
	ifstream fileset("program.hex", ios::in | ios::binary | ios::ate);
	if (fileset)
	{
		
		fileset.seekg(24);
		fileset.read(currCommand, 4); 
		changeEndian(currCommand, 32, 2);
		e_entry = stringToIntInstruction(currCommand, 32);
			
		fileset.seekg(28);
		fileset.read(currCommand, 4); 
		changeEndian(currCommand, 32, 2);
		//char lol[4];
		//lol[0] = currCommand[0];
		////lol[1] = currCommand[1];
		//lol[2] = currCommand[2];
		//lol[3] = currCommand[3];
		e_phoff = stringToIntInstruction(currCommand, 32);
			
		fileset.seekg(32);
		fileset.read(currCommand, 4); 
		changeEndian(currCommand, 32,2);
		e_shoff = stringToIntInstruction(currCommand, 32);
		
		fileset.seekg(42);
		fileset.read(currCommand, 2); 
		changeEndian(currCommand, 16);
		e_phentsize = stringToIntInstruction(currCommand);

		fileset.seekg(44);
		fileset.read(currCommand, 2);
		changeEndian(currCommand, 16);
		e_phnum = stringToIntInstruction(currCommand);

		fileset.seekg(46);
		fileset.read(currCommand, 2);
		changeEndian(currCommand, 16);
		e_shentsize = stringToIntInstruction(currCommand);

		fileset.seekg(48);
		fileset.read(currCommand, 2);
		changeEndian(currCommand, 16);
		e_shnum = stringToIntInstruction(currCommand);
		
		
		//create program header and fill it in
		int progReadAddr = e_phoff;
		for (int i = 0; i < (e_phnum - 1); i++)   //make something for if e_shnum == 0
		{
			//fill in structure
			ProgramHeader prog(&fileset, progReadAddr, currCommand);
			programTable.push_back(prog);
			progReadAddr += e_phentsize;
		}

		
		//create section header and fill it in
		int sectReadAddr = e_shoff;
		for (int i = 0; i < (e_shnum - 1); i++)   //make something for if e_shnum == 0
		{
			//fill in structure
			SectionHeader section(&fileset,sectReadAddr, currCommand);
			sectionTable.push_back(section);
			sectReadAddr += e_shentsize;
		}

		fileset.close();
	}


	// begin sorting through file again 
	ifstream file("program.hex", ios::in | ios::binary | ios::ate);
	if (file)
	{
		cout << "File opened" << endl;

		//find end of file
		file.seekg(0, file.end);
		int end = file.tellg();
		file.seekg(0, file.beg);

		// load instrucVec with all commands
		for (int index = 0; inputeip < end; index++) //iterate 16bits at a time  index is for matching index and inputeip in indexMap
		{
			file.seekg(inputeip);
			file.read(currCommand, 2); //read 16 bits
			changeEndian(currCommand, 16);
			int inputeipstarting = inputeip; // location of current instruction

			numCommand = stringToIntInstruction(currCommand);

			if (is32Bit(numCommand)) //if not true then 32bit instruction, else 16 bit -- checking to see if op1 == 00 (binary)
			{

				lengthCommand = 32;
				inputeip += 4;
				file.read(currCommand + 2, 2); //read next 16 bits  
				//currCommand[4] = '\0';
				changeEndian(currCommand + 2); // convert next 16 bits
				numCommand = numCommand << 16;
				numCommand += stringToIntInstruction(currCommand + 2); //add on next 16 bits
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
				changeEndian(temp);
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
		ofstream outfile("newprogram.hex", ofstream::binary);
		char* buff = new char[4];
		//write to outputfile
		int lineCount = 1;
		for (int k = 0; k < output.size(); k++)   
		{
			output[k].giveInstruction(buff);
			changeEndian(buff, output[k].length());

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
		delete[] buff;


	}
	else
	{
		cout << "Error opening file" << endl;
	}

	//clean up
	cout << "File closed" << endl;
	delete[] currCommand;
	file.close();

	cout << "Done" << endl;


}