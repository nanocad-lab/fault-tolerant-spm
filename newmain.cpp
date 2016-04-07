#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <unordered_set>
#include <string>

using namespace std;

int memory_size = 16384;
char *input_file_path;
char *output_file_path;

int
main (int argc, char *argv[])
{
    /* Parse arguments to program */
    if(argc < 2)
        return EIO;

    input_file_path = argv[1];
    int c;
    while ((c = getopt(argc, argv, "mo:")) != -1){
    	switch (c){
            case 'm':
                memory_size = atoi(argv[optind]);
                break;
            case 'o':
                output_file_path = optarg;
                break;
        }
    }
    printf("Running fault-tolerant-spm with memory constraint as %d bytes\n", memory_size);
    printf("Input file path is %s\n", input_file_path);
    printf("Output file path is %s\n", output_file_path);

    /* Parse addresses */
    FILE* address_file;
    address_file = fopen(input_file_path, "r");
    if (address_file == NULL) {
        printf("Error: address file does not exist in input file path or cannot be opened\n");
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
    fclose(address_file);

    printf("Address file has been closed\n");



    int fcloseerr;
    if((fcloseerr = fclose(address_file)) != 0){
        printf("Failed to close address file\n");
        return fcloseerr;
    }
    printf("Address file has closed\n");
    printf("Program has completed\n");
}
