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

#define SRAM_START 0x20000000
#define SRAM_END 0x3FFFFFFF
#define WORD_SIZE 4

typedef struct DataBin
{
    unsigned int start_address;
    unsigned int size;
} DataBin;

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
align_bad_address(unsigned int &addr){
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
    while ((c = getopt(argc, argv, "m:o:")) != -1) {
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
            default:
                print_usage();
            exit(EXIT_FAILURE);
        }
    }
    if (optind < argc) {
        input_file_path = argv[optind];
        // fprintf(stdout, "optind: %d\n", optind);
        fprintf(stdout, "Input File Path: %s\n", input_file_path);
    }
    printf("Running fault-tolerant-spm with memory constraint as %d bytes\n", memory_size);
    printf("Input file path is %s\n", input_file_path);
    printf("Output file path is %s\n", output_file_path);

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
    printf("Address file has been opened\n");

    /* Load from addresses.txt into bad_addresses*/
    vector<unsigned int> bad_addresses;
    char* address_buff = new char[16];

    printf("Loading addresses\n");
    while (!feof(address_file)) {
        //fscanf(address_file, "0x%8c ", address_buff);
        if (fscanf(address_file, "%s", address_buff) == EOF) {
            break;
        }
        string address_str = address_buff;
        //printf("%s\n", address_str.c_str());
        unsigned int numeric_address = (unsigned int) stoul(address_str, nullptr, 16);
        printf("%d\n", numeric_address);
        //printf("0x%08x\n", numeric_address);
        if (numeric_address < SRAM_START || numeric_address > (SRAM_START + memory_size)) {
            fprintf(stderr, "The address %s is not within SRAM range\n", address_str.c_str());
        }
        else {
            bad_addresses.push_back(numeric_address);
        }
    }

    sort(bad_addresses.begin(), bad_addresses.end());

    //for_each(bad_addresses.begin(), bad_addresses.end(), print_item);

    printf("Addresses successfully loaded\n");
    //printf("There are %u bad addresses\n", (unsigned int)bad_addresses.size());

    //clean up address parsing
    delete[] address_buff;
    int fcloseerr;
    if ((fcloseerr = fclose(address_file)) != 0) {
        fprintf(stderr, "Error: failed to close address file\n");
        return fcloseerr;
    }
    printf("Address file has been closed\n");

    /* SECTION 3: BIN CREATION SECTION
     * This section creates bins given the addresses from the previous section.
     */
    
    //prime the addresses to 32-bit alignment for bad "words"
    for_each(bad_addresses.begin(), bad_addresses.end(), align_bad_address); 
    //for_each(bad_addresses.begin(), bad_addresses.end(), print_item);

    /* copy to set to sift potential duplicates from aligning */
    set<unsigned int> aligned_bad_addresses(bad_addresses.begin(), bad_addresses.end());
    
    printf("There are %u unique bad addresses\n", (unsigned int)aligned_bad_addresses.size());


    //create vector of databins using aligned bad_addresses
    vector<DataBin> data_bins;

    //add in first bin
    
    DataBin first_bin;
    first_bin.start_address = SRAM_START;
    first_bin.size = *(aligned_bad_addresses.begin()) - SRAM_START;
    data_bins.push_back(first_bin);

    for (set<unsigned int>::iterator it = aligned_bad_addresses.begin(); it != aligned_bad_addresses.end(); ++it) {
        DataBin tmpbin;
        tmpbin.start_address = (*it) + WORD_SIZE;

        if (it == aligned_bad_addresses.begin()) {
            DataBin firstbin;
        }

        if (next(it) != aligned_bad_addresses.end()) {
            tmpbin.size = *(next(it)) - tmpbin.start_address;
        }
        else {
            tmpbin.size = (SRAM_START + memory_size) - tmpbin.start_address;
        }
        data_bins.push_back(tmpbin);
    }

    printf("There are %u data bins\n", (unsigned int)data_bins.size());

    //dump data bins here
    for_each(data_bins.begin(), data_bins.end(), [](DataBin x){ print_item(x.start_address); print_item(x.size); });


    /* SECTION ?: ELF PARSING SECTION 
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

    /* SECTION ?: CLEAN UP 
     * This section cleans up the program, indicating a finish.
     */
    printf("Program has completed\n");
}
