/******************************************************************
mapFuncPtr2.cpp
K Zemoudeh
4/5/10

This program improves on mapFuncPtr.cpp by incorporating Exception
Handling. The original program fails on an invalid opcode; this
program doesn't.
*******************************************************************/
#include "assembler.h"
/*
#include <iostream>
#include <string>
#include <map>
#include <stdexcept>

using namespace std;
*/
class NullPointerException: public runtime_error {
public:
	NullPointerException(): runtime_error("Null Pointer!") { }
};

class Assembler {
	typedef void (Assembler::*FP)(string);

	void add(string s) { cout << "add" << s << endl; }
	void sub(string s) { cout << "sub" << s << endl; }
	void load(string s) { cout << "load" << s << endl; }
	map<string, FP> instr;
public:
	Assembler();
	int assemble();
};

Assembler::Assembler()
{
	op["load"] = 0;		op["andi"] = 6;		op["jump"] = 16;
	op["loadi"] = 0;        op["xor"] = 7;          op["jumpl"] = 17;
	op["store"] = 1;	op["xori"] = 7;         op["jumpe"] = 18;
	op["add"] = 2;		op["compl"] = 8;        op["jumpg"] = 19;
	op["addi"] = 2;		op["shl"] = 9;          op["call"] = 20;
	op["addc"] = 3;		op["shla"] = 10;        op["return"] = 21;
	op["addci"] = 3;	op["shr"] = 11;		op["read"] = 22;
	op["sub"] = 4;		op["shra"] = 12;	op["write"] = 23;
	op["subi"] = 4;		op["compr"] = 13;  	op["halt"] = 24;
	op["subc"] = 5;		op["compri"] = 13;	op["noop"] = 25;
	op["subci"] = 5;	op["getstat"] = 14;
        op["and"] = 6;		op["putstat"] = 15;

}

int Assembler::assemble()
{
	string input;

	try {
		cin >> input;
		if (not instr[input] )
			throw NullPointerException();
		else (this->*instr[input])("something");

	} catch (NullPointerException e) {
		cerr << e.what() << endl;
		return 1;
	}
}

main()
{
	Assembler a;
	a.assemble();
}

/*********************************************************************
stringStream.cpp
K Zemoudeh
4/5/10

This program illustrates how an assembly program line could be parsed.
**********************************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

main()
{
	string opcode;
	int rd=-1, rs=-1, constant=-129; // init to invalid values

	fstream assemblyProg;
	assemblyProg.open("prog.s", ios::in);

	string line;
	getline(assemblyProg, line);
	while (!assemblyProg.eof()) {

		// the following line is the trick
		istringstream str(line.c_str());
		str >> opcode >> rd;

		if (opcode == "add")
			str >> rs;
		else if (opcode == "addi")
			str >> constant;
		else cout << "Some other opcode\n";

		cout << opcode << " " << rd << " " << rs << " " << constant << endl;
		getline(assemblyProg, line);
	}
}

