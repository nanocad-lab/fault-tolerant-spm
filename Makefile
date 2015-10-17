#CPP is g++ compiler
CPP = g++

#g++ compiler options
#	-c prevents linking
#	-std=c++11 allows <unordered_set> to be used
#	-I . specifies header file locations
CPPFLAGS = -c -std=c++11
HDRLOC = -I .

#make targets
all: fault-tolerant-spm

fault-tolerant-spm: ELF.o Instruction.o MiscFuncs.o Source.o 
	$(CPP) ELF.o Source.o Instruction.o MiscFuncs.o -o fault-tolerant-spm
	
ELF.o: ELF.cpp ELF.h MiscFuncs.h Instruction.h
	$(CPP) $(CPPFLAGS) ELF.cpp $(HDRLOC)
	
Instruction.o: Instruction.cpp Instruction.h MiscFuncs.h
	$(CPP) $(CPPFLAGS) Instruction.cpp $(HDRLOC)
	
MiscFuncs.o: MiscFuncs.cpp MiscFuncs.h
	$(CPP) $(CPPFLAGS) MiscFuncs.cpp $(HDRLOC)
	
Source.o: Source.cpp Instruction.h MiscFuncs.h ELF.h
	$(CPP) $(CPPFLAGS) Source.cpp $(HDRLOC)
	
clean:
	rm *.o fault-tolerant-spm
