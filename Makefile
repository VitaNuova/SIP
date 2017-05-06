all: mutator.o mutator
mutator.o: mutator.cpp
	g++ -c -I/usr/local/include mutator.cpp -std=c++0x
mutator: mutator.o
	g++ mutator.o -L/usr/local/lib -ldyninstAPI -linstructionAPI -o mutator -std=c++0x
