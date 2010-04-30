/************************************************
 * VirtualMachine.h
 * Patrick O'Connor, Timothy Covel
 * Apr/21/2010
 * See os.cpp for compile instructions
 * 
 * classes and union for virtual operating system instruction set.
 ************************************************/
#ifndef _VIRTUALMACHINE_H
#define	_VIRTUALMACHINE_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector> 
#include <map>
#include <list>
#include <cstdio>
#include <cstdlib>

using namespace std;

struct PCB{
        vector<int> r;
        int pc, sr, sp, base, limit, IO_clock;
        int CPU_time, largest_stack_size, ta_time, io_time, waiting_time;
        string pName;

        ifstream pcbInFile;
        ofstream pcbOutFile;
        fstream pcbStateFile;
};

//from union.cpp
class format1 {
	public:
		unsigned UNUSED:6;//UNUSED[5:0]
		unsigned RS:2;		//RS[7:6]
		unsigned I:1;		//I[8]
		unsigned RD:2;		//RD[10:9]
		unsigned OP:5;		//OP[15:11]
	};

//from union.cpp
class format2 {
	public:
		unsigned AC:8;	//AC[7:0]
		unsigned I:1;		//I[8]
		unsigned RD:2;		//RD[10:9]
		unsigned OP:5;		//OP[15:11]
	};

//from union.cpp less function 3	
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
		void myAnd();
		void myXor();
		void myCompl();
		void shl();
		void shla();
		void shr();
		void shra();
		void compr();
		void getStat();
		void putStat();
		void jump();
		void jumpl();
		void jumpe();
		void jumpg();
		void call();
		void myReturn();
		void read();
		void write();
		void halt();
		void noop();

		void setCarry();
		bool getCarry();
		
		//void run(string);
		void run(PCB *);

		void loadMemory(list<PCB *> &);
		void saveState(PCB *);
		void loadState(PCB *);
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
		int ir, sr, limit, pc, base, sp, clock, counter, timeSlice;
		vector<FP> funcMap;
		PCB * current;
	};
#endif	/* _VIRTUALMACHINE_H */

