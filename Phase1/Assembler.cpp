/******************************************************************
mapFuncPtr2.cpp
K Zemoudeh
4/5/10

This program improves on mapFuncPtr.cpp by incorporating Exception
Handling. The original program fails on an invalid opcode; this
program doesn't.
*******************************************************************/

#include <iostream>
#include <string>
#include <map>
#include <stdexcept>

using namespace std;

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
	instr["add"] = &Assembler::add;
	instr["sub"] = &Assembler::sub;
	instr["load"] = &Assembler::load;
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

