all: mutator.o mutator
mutator.o: mutator.cpp 
	g++ -m32 -g -c -I/usr/include/dyninst mutator.cpp -std=c++0x
mutator: mutator.o
	g++ -m32 -g mutator.o -L/usr/lib/dyninst -ldyninstAPI -linstructionAPI -o mutator -std=c++0x
