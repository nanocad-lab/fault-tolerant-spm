/* C Library Headers */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <elf.h>

/* C++ Library Headers */
#include <unordered_set>
#include <set>
#include <algorithm>
#include <vector>
#include <string>

/* User Defined Headers */
#include "Instruction.h"
#include "MiscFuncs.h"

using namespace std;

#define SRAM_START  0x20000000
#define SRAM_END    0x3FFFFFFF
#define WORD_SIZE   4
//#define VPRINTF( arg ) if (verbose_flag) {printf(arg);}
#define VPRINTF(format, arg)  if (verbose_flag) {printf(format, arg);}
typedef struct DataBin
{
    unsigned int start_address;
    unsigned int size;
} DataBin;


bool    verbose_flag = false;
char    *program_name;
char    *input_file_path;
char    *output_file_path;
int     memory_size = 16384;

void
print_item(unsigned int x)
{
    printf("%d\n", x);
}

void
print_usage()
{
    fprintf(stderr, "Usage: %s [-m memory_size] [-o output] input\n", program_name);
}

void
align_bad_address(unsigned int &addr)
{
    addr -= (addr % WORD_SIZE);
}

int
main(int argc, char *argv[])
{
    /* SECTION 1: OPTION PARSING SECTION
     * This part of the program parses arguments and sets fields
     * accordingly.
     */

    program_name = argv[0];
    /* Parse arguments to program */
    int c;
    while ((c = getopt(argc, argv, "m:o:v")) != -1) {
    	switch (c){
            case 'm':
                memory_size = atoi(optarg);
                break;
            case 'o':
                output_file_path = optarg;
                break;
            case 'i':
                input_file_path = optarg;
                break;
            case 'v':
                verbose_flag = true;
                break;
            default:
                print_usage();
            exit(EXIT_FAILURE);
        }
    }
    if (optind < argc) {
        input_file_path = argv[optind];
        VPRINTF("optind: %d\n", optind);
        VPRINTF("Input File Path: %s\n", input_file_path);
    }
    VPRINTF("Running fault-tolerant-spm with memory constraint as %d bytes\n", memory_size);
    VPRINTF("Input file path is %s\n", input_file_path);
    VPRINTF("Output file path is %s\n", output_file_path);

    /* SECTION 2: ADDRESS PARSING SECTION
     * This part of the program parses an address file given paths from
     * the previous section for option parsing.
     *
     * Addresses.txt is a text file with hex representations of 32-bit 
     * addresses.
     *
     * Each 32-bit address is 8 hex characters prefixed by 0x.
     * Each address is separated from other addresses by newlines or 
     * whitespaces. 
     * 
     * e.g., when Addresses.txt is opened by notepad, it will show:
     * 0x040302AF
     * 0x28EA1423
     * 0x00FF0243
     */

    /* Parse addresses */
    if (input_file_path == NULL) {
        fprintf(stderr, "Input file is required\n");
        print_usage();
        exit(EXIT_FAILURE);
    }
    FILE* address_file;
    address_file = fopen(input_file_path, "r");
    if (address_file == NULL) {
        fprintf(stderr, "Error: address file does not exist in input file path or cannot be opened\n");
        return -1;
    }
    VPRINTF("%s\n", "Address file has been opened");

    /* Load from addresses.txt into bad_addrs*/
    vector<unsigned int> bad_addrs;
    char* address_buff = new char[16];

    VPRINTF("%s", "Loading addresses\n");
    while (!feof(address_file)) {
        //fscanf(address_file, "0x%8c ", address_buff);
        if (fscanf(address_file, "%s", address_buff) == EOF) {
            break;
        }
        string address_str = address_buff;
        //printf("%s\n", address_str.c_str());
        unsigned int numeric_address = (unsigned int) stoul(address_str, nullptr, 16);
        VPRINTF("%d\n", numeric_address);
        //printf("0x%08x\n", numeric_address);
        if (numeric_address < SRAM_START || numeric_address >= (SRAM_START + memory_size)) {
            fprintf(stderr, "The address %s is not within SRAM range\n", address_str.c_str());
        }
        else {
            bad_addrs.push_back(numeric_address);
        }
    }

    sort(bad_addrs.begin(), bad_addrs.end());

    //for_each(bad_addrs.begin(), bad_addrs.end(), print_item);

    VPRINTF("%s\n", "Addresses successfully loaded");
    //printf("There are %u bad addresses\n", (unsigned int)bad_addrs.size());

    //clean up address parsing
    delete[] address_buff;
    int fcloseerr;
    if ((fcloseerr = fclose(address_file)) != 0) {
        fprintf(stderr, "Error: failed to close address file\n");
        return fcloseerr;
    }
    VPRINTF("%s\n", "Address file has been closed");

    /* SECTION 3: DATA BIN CREATION SECTION
     * This section creates bins given the addresses from the previous section.
     */
    
    //prime the addresses to 32-bit alignment for bad "words"
    for_each(bad_addrs.begin(), bad_addrs.end(), align_bad_address); 
    //for_each(bad_addrs.begin(), bad_addrs.end(), print_item);

    /* copy to set to sift potential duplicates from aligning */
    set<unsigned int> aligned_bad_addrs(bad_addrs.begin(), bad_addrs.end());
    
    VPRINTF("There are %u unique bad addresses\n", (unsigned int)aligned_bad_addrs.size());

    //create vector of databins using aligned bad_addrs
    vector<DataBin> data_bins;

    if (data_bins.size() == 0) {
        VPRINTF("%s\n", "No ELF adjustment needed");
        exit(0);
    }
    else {
        //add in first bin
        DataBin first_bin;
        first_bin.start_address = SRAM_START;
        first_bin.size = *(aligned_bad_addrs.begin()) - SRAM_START;
        data_bins.push_back(first_bin);

        for (set<unsigned int>::iterator it = aligned_bad_addrs.begin(); it != aligned_bad_addrs.end(); ++it) {
            DataBin tmpbin;
            tmpbin.start_address = (*it) + WORD_SIZE;

            if (next(it) != aligned_bad_addrs.end()) {
                tmpbin.size = *(next(it)) - tmpbin.start_address;
            }
            else {
                tmpbin.size = (SRAM_START + memory_size) - tmpbin.start_address; //CHECK THIS
            }
            data_bins.push_back(tmpbin);
        }
    }

    /* Number of data bins should be number of bad addresses + 1 */
    printf("There are %u data bins\n", (unsigned int)data_bins.size());

    /* dump data bins for visual confirmation */
    if (verbose_flag) {
        for_each(data_bins.begin(), data_bins.end(), [](DataBin x){ print_item(x.start_address); print_item(x.size); });
    }



    /* SECTION 4: ELF PARSING SECTION 
     * This section reads in the generated ELF file from running 
     * arm-none-eabi-gcc.
     * 
     * The ELF Header is read in and the pertinent information used to 
     * find addresses of the Program Header Table and Section Header Table.
     * Afterwards, the ELF Header info is disposed of other than 
     * information such as the code entry point and the executable's 
     * endianness.
     * 
     * The Program Header Table is the most useful to us since it holds 
     * Program Headers which tell us the location of the .text (aka Code) 
     * segment, Data (initialized variables, etc) segment, and .bss 
     * segment (uninitialized variables).
     *
     * The Section Header Table is probably less useful, since it is 
     * typically only during linking and compiling.
     * 
     * Side note about endianness:
     * In normal situations, the ELF file is the same endianness as the 
     * target machine.
     * ARM specifications store instructions as little endian, but device 
     * memory may be either. (The LPC1768 is little endian)
     */

    //ELF PARSING NEEDS TO BE REWRITTEN USING LINUX LIBRARY ELF.H

    //int input_eip = 0;
    //int actual_eip = 0;
    char* curr_ins = new char[5];
    char* elf_buff = new char[5];
    unsigned int ins; //stoi form of curr_ins
    int ins_size; //size in bytes of current instruction

    char endian;

    //vector<ProgramHeader> programHeaderTable;
    //vector<SectionHeader> sectionHeaderTable;

    // unsigned int beg_code_segment;
    // unsigned int end_code_segment;
    // unsigned int code_entry_address;

    // //load values for ELF variables, load all section headers and program headers
    // ifstream fileset("program.elf", ios::in | ios::binary | ios::ate);
    // if (!fileset){
    //     cout << "Error: program.elf does not exist in the working directory or cannot be opened" << endl;
    //     return -1;
    // }
    // else {
    //     cout << "program.elf has been opened" << endl;
    //     ElfHeader elfHeader(&fileset, 0, elfBuff);
    //     endianness = elfHeader.e_ident[5];

    //     code_entry_address = elfHeader.e_entry; //the address where code execution will begin
    //     cout << showbase;
    //     cout << "entry point address: " << hex << code_entry_address << endl;

    //     //load all of the program headers into programHeaderTable data structure
    //     unsigned int progHdrReadAddr = elfHeader.e_phoff;
    //     cout << "Program header table starts at " << hex << progHdrReadAddr << endl;
    //     cout << "Program header size is " << dec << elfHeader.e_phentsize << " bytes" << endl;
    //     cout << "Program header number is " << dec << elfHeader.e_phnum << " headers" << endl;
    //     if (progHdrReadAddr != 0) { //if e_phnum is 0, then e_phoffm must also be 0
    //         for (int i = 0; i < elfHeader.e_phnum; i++)
    //         {
    //             ProgramHeader programHeader(&fileset, progHdrReadAddr, elfBuff, endianness);
    //             programHeaderTable.push_back(programHeader);
    //             progHdrReadAddr += elfHeader.e_phentsize;
    //         }
    //         beg_code_segment = programHeaderTable.at(0).p_paddr + programHeaderTable.at(0).p_offset; //fairly certain this is correct
    //         end_code_segment = beg_code_segment + programHeaderTable.at(0).p_memsz; //first program section is code section
    //         cout << "Program headers loaded" << endl;
    //         cout << "There are " << dec << programHeaderTable.size() << " entries in the program header table" << endl;
    //     }
    //     else
    //         cout << "Warning: No program header table exists" << endl;

    //     //load all of the section headers into sectionHeaderTable data structure
    //     unsigned int sectReadAddr = elfHeader.e_shoff;
    //     cout << "Section header table starts at " << hex << sectReadAddr << endl;
    //     cout << "Section header size is " << dec << elfHeader.e_shentsize << " bytes" << endl;
    //     cout << "Section header number is " << dec << elfHeader.e_shnum << " headers" << endl;
    //     if (sectReadAddr != 0) { //if e_shnum is 0, then e_shoff must also be 0
    //         for (int i = 0; i < elfHeader.e_shnum; i++)
    //         {
    //             SectionHeader section(&fileset, sectReadAddr, elfBuff, endianness);
    //             sectionHeaderTable.push_back(section);
    //             sectReadAddr += elfHeader.e_shentsize;
    //         }
    //         cout << "Section headers loaded" << endl;
    //         cout << "There are " << dec << sectionHeaderTable.size() << " entries in the section header table" << endl;
    //     }
    //     else
    //         cout << "Warning: No section header table exists" << endl;
    //     fileset.close();
    //     cout << "program.elf has been closed\n" << endl;
    // }
    delete[] curr_ins;
    delete[] elf_buff;







    /* SECTION ?: CLEAN UP 
     * This section cleans up the program, indicating a finish.
     */
    printf("Program has completed\n");
}
