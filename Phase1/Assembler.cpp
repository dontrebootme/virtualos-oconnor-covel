#include "Assembler.h"

/*class NullPointerException: public runtime_error {
public:
	NullPointerException(): runtime_error("Null Pointer!") { }
};

class Assembler {
	typedef void (Assembler::*FP)(string);

	void add(string s) { cout << "add" << s << endl; }
	void sub(string s) { cout << "sub" << s << endl; }
	void load(string s) { cout << "load" << s << endl; }
	map<string, FP> instr;
	Assembler();
	int assemble();
};
*/
Assembler::Assembler()
{
	opcode["load"] = 0;	opcode["andi"] = 6;		opcode["jump"] = 16;
	opcode["loadi"] = 0;    opcode["xor"] = 7;      	opcode["jumpl"] = 17;
	opcode["store"] = 1;	opcode["xori"] = 7;     	opcode["jumpe"] = 18;
	opcode["add"] = 2;	opcode["compl"] = 8;   		opcode["jumpg"] = 19;
	opcode["addi"] = 2;	opcode["shl"] = 9;     		opcode["call"] = 20;
	opcode["addc"] = 3;	opcode["shla"] = 10;    	opcode["return"] = 21;
	opcode["addci"] = 3;	opcode["shr"] = 11;		opcode["read"] = 22;
	opcode["sub"] = 4;	opcode["shra"] = 12;		opcode["write"] = 23;
	opcode["subi"] = 4;	opcode["compr"] = 13;  		opcode["halt"] = 24;
	opcode["subc"] = 5;	opcode["compri"] = 13;		opcode["noop"] = 25;
	opcode["subci"] = 5;	opcode["getstat"] = 14;
        opcode["and"] = 6;	opcode["putstat"] = 15;

}

void Assembler::assemble(string file)
{
	assemblyFile.open(file, ios::in);
	
	//Check if file exists
	if (!assemblyFile)
	{
		cout << "Error opening file: " << file << endl;
		exit(1);
	}
	
	//Remove file extension and add new extension
	outputName.assign(file,0,file.length()-2);
	outputName += ".o";
	objectFile.open(outputName,ios::out);
	
	getline(assemblyFile, line);
        while(!assemblyFile.eof()){
                int rd=-1, rs=-1, constant=-129;

                istringstream str(line.c_str());
                str >> opcode >> rd >> rs;

                switch(opcode)
                {
                case "loadi":
                case "addci":
                case "subi":
                case "subci":
                case "addi":
                case "xori":
                case "compri":
                case "andi":
                        if ( rd >= 0 && rd < 4 && rs >= 0 && rs < 128 )
                        {
                                instrNum = op[opcode] << 11;
                                instrNum += rd << 9;
                                instrNum += 1 << 8;
                                instrNum += rs;
                        }
                        else if ( rd >= 0 && rd < 4 && rs < 0 && rs >= -128 )
                        {
                                instrNum = op[opcode] << 11;
                                instrNum += rd << 9;
                                instrNum += 1 << 9;
                                instrNum += rs;
                        }
                        break;

                case "call":
                case "jumpg":
                case "jumpe":
                case "jumpl":
                case "jump":
                        instrNum = (op[opcode] << 11) + rd;
                        break;

                case "compl":
                case "shl":
                case "shla":
                case "shr":
                case "shra":
                case "getstat":
                case "putstat":
                case "read":
                case "write":
                        instrNum = op[opcode] << 11;
                        instrNum += rd << 9;
                        break;

                case "load":
                case "store":
                        instrNum = op[opcode] << 11;
                        instrNum += rd << 9;
                        instrNum += rs;
                        break;

                case "add":
                case "addc":
                case "sub":
                case "subc":
                case "and":
                case "xor":
                case "compr":
                        instrNum = op[opcode] << 11;
                        instrNum += rd << 9;
                        instrNum += rs << 6;
                        break;

		default:
                        cout << "Failed to assemble the following instruction:   ";

                        if(rd != -1 && rs != -1)
                                cout << opcode << " " << rd << " " << rs << endl;
                        else
                                cout << opcode << endl;

                        if(rd != -1 && rs == -1)
                                cout << opcode << " " << rd << endl;

                        cout << "Assembler is now exiting!\n";
                        exit(1);
                }

                objectFile << instrNum << endl;
                getline(assemblyFile, line);

	}
	assemblyFile.close();
	objectFile.close();
}
/*
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
/*
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
*/
