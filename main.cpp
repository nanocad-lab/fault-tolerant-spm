//C Library Headers
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <unordered_set>
#include <set>
#include <string>
#include <elf.h>

//User Defined Headers
#include "Instruction.h"
#include "MiscFuncs.h"

using namespace std;

int memory_size = 16384;
char *input_file_path;
char *output_file_path;

int
main (int argc, char *argv[])
{
    /* SECTION 1: OPTION PARSING SECTION
     * This part of the program parses arguments and sets fields
     * accordingly.
     */

    /* Parse arguments to program */
    int c;
    while ((c = getopt(argc, argv, "m:o:")) != -1){
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
                fprintf(stderr, "Usage: %s [-m memory_size] [-o output] input\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    if (optind < argc) {
        input_file_path = argv[optind];
    }
    // printf("Running fault-tolerant-spm with memory constraint as %d bytes\n", memory_size);
    // printf("Input file path is %s\n", input_file_path);
    // printf("Output file path is %s\n", output_file_path);

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
    FILE* address_file;
    address_file = fopen(input_file_path, "r");
    if (address_file == NULL) {
        fprintf(stderr, "Error: address file does not exist in input file path or cannot be opened\n");
        return -1;
    }
    printf("Address file has been opened\n");

    /* Load from addresses.txt into bad_addresses*/
    unordered_set<unsigned int> bad_addresses;
    char* address_buff = new char[8];

    printf("Loading addresses\n");
    while (!feof(address_file)){
        fscanf(address_file, "0x%8c ", address_buff);
        string address_str = address_buff;
        printf("%s\n", address_str.c_str());
        unsigned int numeric_address = (unsigned int) stoul(address_str, nullptr, 16);

        bad_addresses.insert(numeric_address);
    }

    printf("Addresses successfully loaded\n");
    printf("There are %u bad addresses\n", (unsigned int)bad_addresses.size());

    //clean up address parsing
    delete[] address_buff;
    int fcloseerr;
    if((fcloseerr = fclose(address_file)) != 0){
        fprintf(stderr, "Error: failed to close address file\n");
        return fcloseerr;
    }
    printf("Address file has been closed\n");


    /* SECTION 3: ELF PARSING SECTION 
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



    /* SECTION ?: CLEAN UP 
     * This section cleans up the program, indicating a finish.
     */
    printf("Program has completed\n");
}
