/* 
 * File:   VirtualMachine.h
 * Author: tcovel
 *
 * Created on April 7, 2010, 2:23 PM
 */

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
		unsigned ADDR:8;	//ADDR[7:0]
		unsigned I:1;		//I[8]
		unsigned RD:2;		//RD[10:9]
		unsigned OP:5;		//OP[15:11]
	};
	
union instruction {
	int i;
	format1 f1;
	format2 f2;
	};


#endif	/* _VIRTUALMACHINE_H */

