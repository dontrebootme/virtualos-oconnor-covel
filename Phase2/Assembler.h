/************************************************
 * Assembler.h
 * Patrick O'Connor, Timothy Covel
 * Apr/21/2010
 * See os.cpp for compile instructions
 * 
 *
 * Assembler Classes  for virtual operating system instruction set.
 * ***********************************************/

#ifndef _ASSEMBLER_H
#define	_ASSEMBLER_H
#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <cstdlib>

using namespace std;
class Assembler {
	map <string, int> opcode;

public:
	Assembler();
	void assemble(string);
private:
	int instrNum;
	string op, line, outputName;
	fstream assemblyFile;
	ofstream objectFile;
};
#endif	
