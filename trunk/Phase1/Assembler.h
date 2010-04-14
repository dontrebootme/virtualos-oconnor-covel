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
	int assemble();
private:
#endif	

