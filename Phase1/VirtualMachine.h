#ifndef _VIRTUALMACHINE_H
#define	_VIRTUALMACHINE_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector> 
#include <map>

using namespace std;

class format1 {
	public:
		unsigned UNUSED:6;//UNUSED[5:0]
		unsigned RS:2;		//RS[7:6]
		unsigned I:1;		//I[8]
		unsigned RD:2;		//RD[10:9]
		unsigned OP:5;		//OP[15:11]
	};

class format2 {
	public:
		unsigned AC:8;	//AC[7:0]
		unsigned I:1;		//I[8]
		unsigned RD:2;		//RD[10:9]
		unsigned OP:5;		//OP[15:11]
	};
	
union instruction {
	int i;
	format1 f1;
	format2 f2;
	};

class VirtualMachine {
	public:
		VirtualMachine();
		
		void load();
		void store();
		void add();
		void addc();
		void sub();
		void subc();
		void and_();
		void xor_();
		void compl_();
		void shl();
		void shla();
		void shr();
		void shra();
		void compr();
		void getstat();
		void putstat();
		void jump();
		void jumpl();
		void jumpe();
		void jumpg();
		void call();
		void return_();
		void read();
		void write();
		void halt();
		void noop();
		void setcarry();
		bool getcarry();
		void run(string);
		
	private:
		typedef void (VirtualMachine::*FP)();
		vector<int> mem;
		vector<int> r;
		instruction objCode;
		string wfile;
		string rfile;
		ifstream dotO_file;
		ifstream dotIn_file;
		ofstream dotOut_file;
		int ir, sr, limit, pc, base, sp, clock;
		vector<FP> funcMap;
	};
#endif	/* _VIRTUALMACHINE_H */

