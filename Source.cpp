
#include "Instruction.h"
#include "MiscFuncs.h"
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
	unordered_map<int, int> addressMap; // inputeip, actualeip
	vector<Instruction> input;
	vector<Instruction> output;

	Instruction space("space", "0000", 16);


	// begin sorting through code
	ifstream file("program.hex", ios::in | ios::binary | ios::ate);
	if (file)
	{
		cout << "File opened" << endl;

		//find end of file
		file.seekg(0, file.end);
		int end = file.tellg();
		file.seekg(0, file.beg);
		//ofstream outfile("newprogram.hex", ofstream::binary);

		// load instrucVec with all commands
		for (int index = 0; inputeip < end; index++) //iterate 16bits at a time  index is for matching index and inputeip in indexMap
		{
			file.seekg(inputeip);
			file.read(currCommand, 2); //read 16 bits
			changeEndian(currCommand, 16);
						
			//figure out if 16 or 32 bit and create appropriate instruction object
			// if 32 bit retrieve rest of command and update eips appropriately
						
			numCommand = stringToIntInstruction(currCommand);
			
			if (is32Bit(numCommand)) //if not true then 32bit instruction, else 16 bit -- checking to see if op1 == 00 (binary)
			{
				
				lengthCommand = 32;
				inputeip += 4;
				file.read(currCommand + 2, 2); //read next 16 bits  
				//currCommand[4] = '\0';
				changeEndian(currCommand + 2); // convert next 16 bits
				numCommand = numCommand << 16;
				numCommand += stringToIntInstruction(currCommand+2); //add on next 16 bits
			}
			else
			{
				lengthCommand = 16;
				inputeip += 2;
			}

			
			string stringcommand = int_to_hexString(numCommand, lengthCommand / 4);

			//create and push instrucs into tempvec
			commandType = typeOfInstruction(numCommand, lengthCommand);
			Instruction currInstruc(commandType,stringcommand, lengthCommand);
			input.push_back(currInstruc);
			indexMap.insert(make_pair(index, inputeip)); // index, inputeip
			
		}
		

		
		//reset loop and go through loop again
		// insert extra branches and space
		// adjust offsets
		for (int i = 0, inputeip = 0; inputeip < end;)
		{
			int instrucLength = input[i].length();
			inputeip += instrucLength / 8;
			int startingeip = actualeip;
			int tempVectorIndex = i;
			int bytesReplaced = 0;
			while (invalidAddressDetected(badAddresses, actualeip, 2 * bytesReplaced))
			{
				bytesReplaced += input[tempVectorIndex].length()/8;
				actualeip += bytesReplaced; // this space will be taken by the space instruction (bytesReplaced)
				tempVectorIndex++;
			}

			if (bytesReplaced > 0)
			{
				//check what branch length to use

				//if 16bit branch is required
				int imm = (actualeip - startingeip + 4) >> 2; //(target - PC) >> 2
				int encoding = 0XE000 + imm;
				Instruction branch("insertedbranch", int_to_hexString(encoding, 4), 16);
				output.push_back(branch);
				actualeip += 2;

				//if 32bit branch is required

				//actual eip for this temp already taken care of in above while loop checking for invalid addresses
				
				for (int a = 0; a < bytesReplaced/2; a += 1)
				{
					output.push_back(space);
				}

				for (int j = 0; j < bytesReplaced; )
				{
					output.push_back(input[i]);
					//find out input eip
					inputeip = indexMap.find(i)->second;   //should add a safety check here
					//insert inputeip and actual pair to addressmap
					addressMap.insert(make_pair(inputeip, actualeip));
					actualeip += input[i].length() / 8;
					j += input[i].length() /8;
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
				actualeip += input[i].length();
				i++;
			}
		}
		
		int limit = output.size();
		for (int n = 0; n < limit; n++)
		{
			if (output[n].type() == "branch")
			{
				string strimm = output[n].giveInstruction();
				int command = atoi((char *) &strimm);
				//int imm = (actualeip - startingeip + 4) >> 2;  //reverse engineer this
			}
		}

		
		ofstream outfile("newprogram.hex", ofstream::binary);
		char* buff = new char[8];
		//write to outputfile
		string tempstr;
		int lineCount = 1;
		for (int k = 0; k < output.size(); k++)   //change input to out for final just doing this for test
		{
			tempstr = output[k].giveInstruction();
			for (int j = 0; j < tempstr.size(); j++)
			{
				buff[j] = tempstr[j];
				
			}
			
		changeEndian(buff, output[k].length(),2);

			if (output[k].length() == 16)
			{
				outfile.write(buff,  4);
				outfile << " ";
				if (lineCount == 8)
				{
					outfile << endl;
					lineCount = 0;
				}
				lineCount++;
			}
			else
			{
				outfile.write(buff, 4);
				outfile << " ";
				if (lineCount == 8)
				{
					outfile << endl;
					lineCount = 0;
				}
				lineCount++;
				outfile.write(buff+4, 4);
				outfile << " ";
				if (lineCount == 8)
				{
					outfile << endl;
					lineCount = 0;
				}
				lineCount++;
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