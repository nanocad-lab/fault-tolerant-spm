#CPP is g++ compiler
CPP = g++

#g++ compiler options
#	-c prevents linking
#	-std=c++11 allows <unordered_set> to be used
#	-I . specifies header file locations
CPPFLAGS = -std=c++11
OBJFLAGS = -c
HDRLOC = -I .

SOURCES = ELF.cpp Instruction.cpp MiscFuncs.cpp main.cpp
OBJECTS = ELF.o Instruction.o MiscFuncs.o main.o

#make targets
all: fault-tolerant-spm

fault-tolerant-spm: ELF.o Instruction.o MiscFuncs.o main.o 
	$(CPP) $(CPPFLAGS) $(SOURCES) -o fault-tolerant-spm
	
ELF.o: ELF.cpp ELF.h MiscFuncs.h Instruction.h
	$(CPP) $(CPPFLAGS) $(OBJFLAGS) ELF.cpp $(HDRLOC)
	
Instruction.o: Instruction.cpp Instruction.h MiscFuncs.h
	$(CPP) $(CPPFLAGS) $(OBJFLAGS) Instruction.cpp $(HDRLOC)
	
MiscFuncs.o: MiscFuncs.cpp MiscFuncs.h
	$(CPP) $(CPPFLAGS) $(OBJFLAGS) MiscFuncs.cpp $(HDRLOC)
	
main.o: main.cpp Instruction.h MiscFuncs.h ELF.h
	$(CPP) $(CPPFLAGS) $(OBJFLAGS) main.cpp $(HDRLOC)
	
clean:
	-@rm *.o fault-tolerant-spm 2>/dev/null || true

.PHONY: all clean
