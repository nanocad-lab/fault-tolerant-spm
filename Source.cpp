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
#include <stdint.h>
using namespace std;

int main()
{
	/* Entire code could be made more clear by: 
	 * 1) Using the stdint.h typedefs
	 * 2) Separating the numbered code sections below into functions for readability
	 * 
	 * The whole "numericInstruction" thing may be a bit of a misnomer, it just stores the bit representation of the instruction directly into memory
	 * which isn't really a "number"
	 */
	

	/* SECTION 1: ADDRESS PARSING SECTION
	 * Addresses.txt is a text file with hex representations of 32-bit addresses.
	 * Each 32-bit address is 8 hex characters prefixed by 0x.
	 * Each address is separated from other addresses by newlines or whitespaces. 
	 * e.g., when Addresses.txt is opened by notepad, it will show:
	 * 0x040302AF
	 * 0x28EA1423
	 * 0x00FF0243
	 */

	/* Open addresses.txt */
	FILE* addressFile;
	addressFile = fopen("addresses.txt", "r");
	if (addressFile == NULL) {
		cout << "Error: addresses.txt does not exist in the working directory or cannot be opened" << endl;
		cout << "Your working directory is: ";
		system("cd"); //does not work on Linux
		cout << endl;
		return -1;
	}
	cout << "addresses.txt has been opened" << endl;

	/* Load from addresses.txt into badAddresses*/
	unordered_set<unsigned int> badAddresses;
	char* addressBuff = new char[8];

	cout << "Loading addresses" << endl;
	while (!feof(addressFile)){
		int scanVal = fscanf(addressFile, "0x%8c ", addressBuff);
		string addressString(addressBuff);
		unsigned int intAddress = (unsigned int) stoul(addressString, nullptr, 16);

		badAddresses.insert(intAddress);
	}

	cout << "Addresses successfully loaded" << endl;
	cout << "There are " << badAddresses.size() << " bad addresses" << endl;

	//clean up address parsing
	delete[] addressBuff;
	fclose(addressFile);
	cout << "addresses.txt has been closed\n" << endl;;


	/* SECTION 2: ELF PARSING SECTION 
	 * This section reads in the generated ELF file from running arm-none-eabi-gcc.
	 * 
	 * The ELF Header is read in and the pertinent information used to find addresses of the Program Header Table and Section Header Table.
	 * Afterwards, the ELF Header info is disposed of other than information such as the code entry point and the executable's endianness.
	 * 
	 * The Program Header Table is the most useful to us since it holds Program Headers which tell us the location of the .text (aka Code) segment,
	 * Data (initialized variables, etc) segment, and .bss segment (uninitialized variables).
	 *
	 * The Section Header Table is probably less useful, since it is typically only during linking and compiling.
	 * 
	 * Side note about endianness:
	 * In normal situations, the ELF file is the same endianness as the target machine.
	 * ARM specifications store instructions as little endian, but device memory may be either. (The LPC1768 is little endian)
	 */

	int input_eip = 0;
	int actual_eip = 0;
	char* currInstruction = new char[5];
	char* elfBuff = new char[5];
	unsigned int numericInstruction = 0; //32-bit form of currInstruction
	int commandLengthInBytes = 0; //length of the current command

	char endianness;

	vector<ProgramHeader> programHeaderTable;
	vector<SectionHeader> sectionHeaderTable;

	unsigned int beg_code_segment;
	unsigned int end_code_segment;
	unsigned int code_entry_address;

	//load values for ELF variables, load all section headers and program headers
	ifstream fileset("program.elf", ios::in | ios::binary | ios::ate);
	if (!fileset){
		cout << "Error: program.elf does not exist in the working directory or cannot be opened" << endl;
		return -1;
	}
	else {
		cout << "program.elf has been opened" << endl;
		ElfHeader elfHeader(&fileset, 0, elfBuff);
		endianness = elfHeader.e_ident[5];

		code_entry_address = elfHeader.e_entry; //the address where code execution will begin
		cout << showbase;
		cout << "entry point address: " << hex << code_entry_address << endl;

		//load all of the program headers into programHeaderTable data structure
		unsigned int progHdrReadAddr = elfHeader.e_phoff;
		cout << "Program header table starts at " << hex << progHdrReadAddr << endl;
		cout << "Program header size is " << dec << elfHeader.e_phentsize << " bytes" << endl;
		cout << "Program header number is " << dec << elfHeader.e_phnum << " headers" << endl;
		if (progHdrReadAddr != 0) { //if e_phnum is 0, then e_phoffm must also be 0
			for (int i = 0; i < elfHeader.e_phnum; i++)
			{
				ProgramHeader programHeader(&fileset, progHdrReadAddr, elfBuff, endianness);
				programHeaderTable.push_back(programHeader);
				progHdrReadAddr += elfHeader.e_phentsize;
			}
			beg_code_segment = programHeaderTable.at(0).p_paddr + programHeaderTable.at(0).p_offset; //fairly certain this is correct
			end_code_segment = beg_code_segment + programHeaderTable.at(0).p_memsz; //first program section is code section
			cout << "Program headers loaded" << endl;
			cout << "There are " << dec << programHeaderTable.size() << " entries in the program header table" << endl;
		}
		else
			cout << "Warning: No program header table exists" << endl;

		//load all of the section headers into sectionHeaderTable data structure
		unsigned int sectReadAddr = elfHeader.e_shoff;
		cout << "Section header table starts at " << hex << sectReadAddr << endl;
		cout << "Section header size is " << dec << elfHeader.e_shentsize << " bytes" << endl;
		cout << "Section header number is " << dec << elfHeader.e_shnum << " headers" << endl;
		if (sectReadAddr != 0) { //if e_shnum is 0, then e_shoff must also be 0
			for (int i = 0; i < elfHeader.e_shnum; i++)
			{
				SectionHeader section(&fileset, sectReadAddr, elfBuff, endianness);
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
	delete[] elfBuff;


	/* Might need a check here for if ELF Header is mapped into memory at a bad address.
	 * I'm not sure if the ELF Header is stripped in the conversion from ELF to BIN.
	 */



	/* SECTION 3: INSTRUCTIONS LOADING SECTION
	 * This section of code loads the instructions from the .text segment into an Instruction vector, which will later be adjusted.
	 * I parse instructions in the ELF file from beg_code_segment to end_code_segment.
	 * This means ARM environment variables are read in since those are located ahead of the code entry point.
	 *
	 * code_entry_address variable holds the actual address in memory of the entry point, beg_code_segment and end_code_segments are locations in
	 * the ELF file.
	 */

	//load all instructions into data structures
	vector<Instruction> instructionsVector;
	ifstream file("program.elf", ios::in | ios::binary | ios::ate);
	if (!file){
		cout << "Error: program.elf does not exist in the working directory or cannot be opened" << endl;
		return -1;
	}
	else {
		cout << "program.elf has been opened" << endl;

		if (endianness == 2) {
			cout << "Error: Can't handle big endian right now." << endl;
			return -1;
		}

		// load instrucVec with all instructions
		// input_eip = beg_code_segment;
		int mem_offset = 0;
		unsigned int addressInMemory = beg_code_segment;
		while ((beg_code_segment + mem_offset) < end_code_segment){
			int new_mem_offset = mem_offset;
			file.seekg(beg_code_segment + mem_offset);
			file.read(currInstruction, 2); //read 2 bytes

			numericInstruction = stringToNumericInstruction(currInstruction, 2, endianness);

			addressInMemory = beg_code_segment + mem_offset;


			/* If the address read in is found to be 32 - bits, read in a 32 - bit instruction and move on to the instruction 4 - bytes over
			 * Otherwise if the address read in is found to be 16 - bits, read in the 16 - bit instruction and move on to the instruction 2 - bytes over
			 */

			if (is32Bit(numericInstruction)){
				commandLengthInBytes = 4;
				new_mem_offset += 4;
				file.read(currInstruction + 2, 2);
				changeEndian(currInstruction + 2, 2);
				numericInstruction = numericInstruction << 16; //shift 16 bits (half-word) to left
				numericInstruction |= stringToNumericInstruction(currInstruction + 2, 2, endianness);
			}
			else{
				commandLengthInBytes = 2;
				new_mem_offset += 2;
			}

			Instruction currInstruction(numericInstruction, commandLengthInBytes, addressInMemory);

			mem_offset = new_mem_offset;

			instructionsVector.push_back(currInstruction);

			/* Leftover mapping stuff from before -- was clunky and too confusing*/
			/*
			int index = 0;
			string stringCommand = int_to_hexString(numericInstruction, commandLengthInBytes*2);
			//create and push instrucs into tempvec
			instructionType = typeOfInstruction(numericInstruction, commandLengthInBytes);

			Instruction currInstruc(instructionType, currInstruction, commandLengthInBytes);
			input.push_back(currInstruc);
			indexMap.insert(make_pair(index, beg_code_segment + offset)); // index, input_eip

			index++;
			offset = new_offset;
			*/
		}
	}

	/* TO-DO Sections Below
	 */

	/* CODE SECTION 4: INSTRUCTIONS ADJUSTMENT SECTION
	 *
	 * Originally there was a whole mapping scheme going on here. My last version had three loops:
	 * The first loop went linearly through the entire instructionsVector and inserted noops
	 * The second loop went through and updated instructions based on a referenced instruction
	 *		e.g. inside Instruction A, there is a pointer to Instruction B
	 *			if Instruction A is going to jump to Instruction B and Instruction B's memory location has changed, then
	 *			Instruction A's bit representation will be changed to jump to the new location of Instruction B
	 * 
	 * Suggestion from Mark: Organize by logical blocks, where each logical block ends with a jump or branch
	 */

	/* CODE SECTION 5: Rebuild the ELF File
	 * Section needs to be redon
	 * Didn't get to this section, needs to be redone.
	 */



		
		/*
		offset = 0;
		index = 0;
		while ((beg_code_segment + offset) < end_code_segment){
			int instructionLength = input[index].size(); //this is in bytes
			int newoffset = offset + instructionLength;
			offset++;
		}

		for (int i = 0, input_eip = beg_code_segment; i < input.size();)
		{
			int instrucLength = input[i].size();
			input_eip += instrucLength;
			int startingeip = actual_eip;
			int tempVectorIndex = i;
			int bytesReplaced = 0;
			while (invalidAddressDetected(badAddresses, actual_eip, 2 * bytesReplaced + 4))
			{
				bytesReplaced += 8; //block out 32 bits if invalid

				actual_eip += 8; //this space will be taken by the space instruction (bytesReplaced)
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
					output.push_back(FourByteNoOp);
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
					actual_eip += input[i].size() / 8;
					j += input[i].size() / 8;
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
				actual_eip += input[i].size()/8;
				i++;
			}
		}

		//adjust branch offsets
		int limit = output.size();
		for (int n = 0; n < limit; n++)
		{

			if (output[n].type().find("branch16conditional") != string::npos)
			{
				int oldinstruction = output[n].getNumericInstruction();
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
				int oldinstruction = output[n].getNumericInstruction();
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
				int oldinstruction = output[n].getNumericInstruction();
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
				int oldinstruction = output[n].getNumericInstruction();
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
				int oldinstruction = output[n].getNumericInstruction();
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
			output[k].getInstruction(buff);
			changeEndian(buff, sizeof(buff)/sizeof(buff[0]));

			if (output[k].size() == 16)
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
	*/
	
	
	//clean up
	delete[] currInstruction;
	file.close();
	cout << "program.elf has been closed\n" << endl;

	cout << "Done" << endl;
}
