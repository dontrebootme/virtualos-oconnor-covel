/************************************************
 * VirtualMachine.cpp
 * Patrick O'Connor, Timothy Covel
 * Apr/21/2010
 * See os.cpp for compile instructions
 * Depends on VirtualMachine.h and Assembled instructions
 *
 * Virtual Machine for Processing Assembled Instructions
 * ***********************************************/
#include "VirtualMachine.h"

void VirtualMachine::setCarry()			
{
	
	if(r[objCode.f1.RD] & 0x00010000) //check if 17th bit of RD is 1
		sr = sr | 0x1; //If it isnt set, set it
	else 
		sr = sr & 0x0000001E; //else Carry is 0
}

bool VirtualMachine::getCarry()	//return 1 if Carry flag is set
{											
	if (sr & 1) // check if Carry flag is 1
		return true; // Carry Flag is Set
	else 	   //otherwise
		return false; //Carry Flag is not Set
}

void VirtualMachine::run(string file)
{
	int temp;
	wfile.assign(file,0,file.length()-2);
	rfile=wfile;
	rfile+=".in";
	wfile+=".out";	
	
	file.erase(file.end()-2,file.end()); // remove last two characters of the filename (e.g prog.s -> prog)
	file += ".o";	//add .o extension to filename (e.g prog -> prog.o)
	
	dotO_file.open(file.c_str(), ios::in);//open .o file for reading
	dotIn_file.open(rfile.c_str(),ios::in);//open .in file for reading
	dotOut_file.open(wfile.c_str(), ios::out);//open .out file for writing
	
	cout << "Now reading " << file << " file for instructions" << endl;
	for(; dotO_file >> temp; limit++) //loading mem with program
		mem[limit] = temp;

	for(;;)//entering infinit loop of fetching ins
	{
		ir = mem[pc];
		pc++;
		objCode.i = ir;
		(this->*funcMap[objCode.f1.OP])();
		
		if(sp < (limit + 6))
		{
			dotOut_file << "Memory is full!\n";
			break;
		}
//Used for debugging
//		cout << "Clock: " << clock << "\tOpcode: " << objCode.f1.OP << endl;
		if(objCode.i == 49152) break;
	}
	dotOut_file << "Clock cycles: " << clock << endl; 
	cout << "Clock cycles: " << clock << endl; 
	cout << "Writing to .out file " << endl;
	
	dotO_file.close();
	dotOut_file.close();
	dotIn_file.close();
}

VirtualMachine::VirtualMachine()
{
	//initializing data types
	funcMap.reserve(26);
	mem.reserve(256);
	r.reserve(4);
	clock = sr = base = pc = limit = 0;
	sp = 256;

	//building function map in a vector
	funcMap[0] = &VirtualMachine::load; 		funcMap[1] = &VirtualMachine::store;
	funcMap[2] = &VirtualMachine::add;		funcMap[3] = &VirtualMachine::addc;
	funcMap[4] = &VirtualMachine::sub;		funcMap[5] = &VirtualMachine::subc;
	funcMap[6] = &VirtualMachine::myAnd;		funcMap[7] = &VirtualMachine::myXor;
	funcMap[8] = &VirtualMachine::myCompl;		funcMap[9] = &VirtualMachine::shl;
	funcMap[10] = &VirtualMachine::shla;		funcMap[11] = &VirtualMachine::shr;
	funcMap[12] = &VirtualMachine::shra;		funcMap[13] = &VirtualMachine::compr;
	funcMap[14] = &VirtualMachine::getStat;		funcMap[15] = &VirtualMachine::putStat;
	funcMap[16] = &VirtualMachine::jump;		funcMap[17] = &VirtualMachine::jumpl;
	funcMap[18] = &VirtualMachine::jumpe;		funcMap[19] = &VirtualMachine::jumpg;
	funcMap[20] = &VirtualMachine::call;		funcMap[21] = &VirtualMachine::myReturn;
	funcMap[22] = &VirtualMachine::read;		funcMap[23] = &VirtualMachine::write;
	funcMap[24] = &VirtualMachine::halt;		funcMap[25] = &VirtualMachine::noop;

}

void VirtualMachine::load() 
{
	if ( objCode.f1.I == 0 ) //I=0
		{
			r[objCode.f2.RD] = mem[objCode.f2.AC];
			clock += 4;
		}
	else //I = 1
		{
			if ((objCode.f2.AC & 0x80) > 0)
				{
					r[objCode.f2.RD] = (objCode.f2.AC | 0xffffff00);
				}
			else
				{
				r[objCode.f2.RD] = objCode.f2.AC;
				}
			clock += 1;
		}
}
		
void VirtualMachine::store() 
{
	clock += 4;
	mem[objCode.f2.AC] = r[objCode.f2.RD];
}
		
void VirtualMachine::add() 
{
	clock += 1;
	if ( objCode.f1.I == 0 )//I = 0
	{
		
		if(objCode.f1.RD >= 0 && objCode.f1.RS >=0 && ((objCode.f1.RD + objCode.f1.RS) < 0))
			sr = sr | 0x00000010;//seting overflow flag
		else if(objCode.f1.RD < 0 && objCode.f1.RS < 0 && ((objCode.f1.RD + objCode.f1.RS) >= 0))
			sr = sr | 0x00000010;

		r[objCode.f1.RD] += r[objCode.f1.RS];
		setCarry();
     }
	else {
			
		if(objCode.f2.RD >= 0 && objCode.f2.AC >=0 && ((objCode.f2.RD + objCode.f2.AC) < 0))
			sr = sr | 0x00000010;
		else if(objCode.f2.RD < 0 && objCode.f2.AC < 0 && ((objCode.f2.RD + objCode.f2.AC) >= 0))
			sr = sr | 0x00000010;
				
		if ((objCode.f2.AC & 0x80) > 0)
			{
				r[objCode.f2.RD] += (objCode.f2.AC | 0xffffff00);
			}
			else
				r[objCode.f2.RD] += objCode.f2.AC;
		setCarry();
	}
}	

void VirtualMachine::addc() 
{
	clock += 1;
	if ( objCode.f1.I == 0 ) //I=0
	{	
		if(objCode.f1.RD >= 0 && objCode.f1.RS >=0 && ((objCode.f1.RD + objCode.f1.RS) < 0))
			sr = sr | 0x00000010;//seting overflow flag
		else if(objCode.f1.RD < 0 && objCode.f1.RS < 0 && ((objCode.f1.RD + objCode.f1.RS) >= 0))
			sr = sr | 0x00000010;
		
		if ( getCarry() ) 
			r[objCode.f1.RD] += r[objCode.f1.RS] + 1;
		else//Carry is not set
			r[objCode.f1.RD] += r[objCode.f1.RS];
		
		setCarry();
    }
	else //I=1
	{
		if(objCode.f2.RD >= 0 && objCode.f2.AC >=0 && ((objCode.f2.RD + objCode.f2.AC) < 0))
			sr = sr | 0x00000010;//seting overflow flag
		else if(objCode.f2.RD < 0 && objCode.f2.AC < 0 && ((objCode.f2.RD + objCode.f2.AC) >= 0))
			sr = sr | 0x00000010;
		
		if ( getCarry() )
		 { 
			if ((objCode.f2.AC & 0x80) > 0)
			{
				r[objCode.f2.RD] += (objCode.f2.AC | 0xffffff00) + 1;
			}
			else
				r[objCode.f2.RD] += objCode.f2.AC + 1;
		}
		else//Carry is not set
		{
			if ((objCode.f2.AC & 0x80) > 0)
			{
				r[objCode.f2.RD] += (objCode.f2.AC | 0xffffff00);
			}
			else
				r[objCode.f2.RD] += objCode.f2.AC;
		}
		setCarry();
	}
}
	
void VirtualMachine::sub() 
{
	clock += 1;
	if ( objCode.f1.I == 0 )//I=0
	{	
		if(objCode.f1.RD >= 0 && objCode.f1.RS >=0 && ((objCode.f1.RD + objCode.f1.RS) < 0))
			sr = sr | 0x00000010;//seting overflow flag
		else if(objCode.f1.RD < 0 && objCode.f1.RS < 0 && ((objCode.f1.RD + objCode.f1.RS) >= 0))
			sr = sr | 0x00000010;

		r[objCode.f1.RD] -= r[objCode.f1.RS];
		setCarry();
   }
	else//I=1
	{	 
		if(objCode.f2.RD >= 0 && objCode.f2.AC >=0 && ((objCode.f2.RD + objCode.f2.AC) < 0))
			sr = sr | 0x00000010;//seting overflow flag
		else if(objCode.f2.RD < 0 && objCode.f2.AC < 0 && ((objCode.f2.RD + objCode.f2.AC) >= 0))
			sr = sr | 0x00000010;
		
		if ((objCode.f2.AC & 0x80) > 0)
		{
			r[objCode.f2.RD] -= (objCode.f2.AC | 0xffffff00);
		}
		else
			r[objCode.f2.RD] -= objCode.f2.AC;
		setCarry();
	}
}	
	
void VirtualMachine::subc()
{
	clock += 1;
	if ( objCode.f1.I == 0 )//I=0
	{
		if(objCode.f1.RD >= 0 && objCode.f1.RS >=0 && ((objCode.f1.RD + objCode.f1.RS) < 0))
			sr = sr | 0x00000010;//seting overflow flag
		else if(objCode.f1.RD < 0 && objCode.f1.RS < 0 && ((objCode.f1.RD + objCode.f1.RS) >= 0))
			sr = sr | 0x00000010;
		
		if ( getCarry() ) 
			r[objCode.f1.RD] -= r[objCode.f1.RS] - 1;
		else//no Carry
			r[objCode.f1.RD] -= r[objCode.f1.RS];

		setCarry();
    }
	else //I=1
	{
      if(objCode.f2.RD >= 0 && objCode.f2.AC >=0 && ((objCode.f2.RD + objCode.f2.AC) < 0))
         sr = sr | 0x00000010;//seting overflow flag
      else if(objCode.f2.RD < 0 && objCode.f2.AC < 0 && ((objCode.f2.RD + objCode.f2.AC) >= 0))
         sr = sr | 0x00000010;
	
		if ( getCarry() ) 
			if ((objCode.f2.AC & 0x80) > 0)
			{
				r[objCode.f2.RD] -= (objCode.f2.AC | 0xffffff00) - 1;
			}
			else
				r[objCode.f2.RD] -= objCode.f2.AC - 1;
		else//no Carry
			if ((objCode.f2.AC & 0x80) > 0)
			{
				r[objCode.f2.RD] -= (objCode.f2.AC | 0xffffff00);
			}
			else
				r[objCode.f2.RD] -= objCode.f2.AC;

		setCarry();
	}
}	
	
void VirtualMachine::myAnd()
{
	clock += 1;
	if ( objCode.f1.I == 0 ) //I=0
		r[objCode.f1.RD] = r[objCode.f1.RD] & r[objCode.f1.RS];
	else //I=1
		if ((objCode.f2.AC & 0x80) > 0)
		{
			r[objCode.f2.RD] = r[objCode.f2.RD] & (objCode.f2.AC | 0xffffff00);
		}
		else
		r[objCode.f2.RD] = r[objCode.f2.RD] & objCode.f2.AC;
}	


void VirtualMachine::myXor()
{
	clock += 1;
	if ( objCode.f1.I == 0 ) 
		r[objCode.f1.RD] = r[objCode.f1.RD] ^ r[objCode.f1.RS];
	else 
		if ((objCode.f2.AC & 0x80) > 0)
		{
			r[objCode.f2.RD] = r[objCode.f2.RD] ^ (objCode.f2.AC | 0xffffff00);
		}
		else
		r[objCode.f2.RD] = r[objCode.f2.RD] ^ objCode.f2.AC;
}		

void VirtualMachine::myCompl()
{
	clock += 1;
	r[objCode.f1.RD] = ~r[objCode.f1.RD];
}

void VirtualMachine::shl()
{
	clock += 1;
	r[objCode.f1.RD] = r[objCode.f1.RD] << 1;
	setCarry();
}

void VirtualMachine::shla()
{
	clock += 1;
	if ( r[objCode.f1.RD] < 0 ) {//check if r[RD] is negative
		r[objCode.f1.RD] = r[objCode.f1.RD] << 1;
		r[objCode.f1.RD] = r[objCode.f1.RD] | 0x80000000;//fixing the sign bit to 1
	}//r[RD] >= 0
	else 
	{
		r[objCode.f1.RD] = r[objCode.f1.RD] << 1;
		r[objCode.f1.RD] = r[objCode.f1.RD] & 0x7FFFFFFF;//fixing the sign bit to 0
	}

	setCarry();
}    

void VirtualMachine::shr()
{
	clock += 1;
	if ( r[objCode.f1.RD] & 1)//check if 1st bit is 1
		sr = sr | 1;//set Carry if 1st bit is 1

	r[objCode.f1.RD] = r[objCode.f1.RD] >> 1;
}       

void VirtualMachine::shra()
{
	clock += 1;
	if(r[objCode.f1.RD] & 1)// check if 1st bit is 1
		sr = sr | 1;//set Carry if 1st bit is 1
			
	if ( r[objCode.f1.RD] < 0 ) {//check if r[RD] is negative
		r[objCode.f1.RD] = r[objCode.f1.RD] >> 1;
		r[objCode.f1.RD] = r[objCode.f1.RD] | 0x80000000;//fixing the sign bit to 1
	}
		else r[objCode.f1.RD] = r[objCode.f1.RD] >> 1; 
}   

void VirtualMachine::compr()
{
	clock += 1;
	if ( objCode.f1.I == 0 ) {//I=0
		if (r[objCode.f1.RD] < r[objCode.f1.RS]) {// less
			sr = sr | 8;//set less flag
			sr = sr & 0x00000019;//reset greater, equal flag
		}
		else if (r[objCode.f1.RD] == r[objCode.f1.RS]) {//equal
			sr = sr | 4;//set equal flag
			sr = sr & 0x00000015;//reset greater, less flags
		}
		else {//greater
			sr = sr | 2;//set greater flag
			sr = sr & 0x00000013;//reset equal, less flags
		}
	}
	else {//compri() when I is 1
		if (r[objCode.f2.RD] < objCode.f2.AC) {//less
			sr = sr | 8;//set less flag
			sr = sr & 0x00000019;//reset greater, equal flag
		}
		else if (r[objCode.f2.RD] == objCode.f2.AC) {//equal
			sr = sr | 4;//set equal flag
			sr = sr & 0x00000015;//reset greater, less flags
		}
		else {//greater
			sr = sr | 2;//set greater flag
			sr = sr & 0x00000013;//reset equal, less flags
		}
	}
}    

void VirtualMachine::getStat()
{
	clock += 1;
	r[objCode.f1.RD] = sr;
}

void VirtualMachine::putStat()
{
	clock += 1;
	sr = r[objCode.f1.RD];
}   

void VirtualMachine::jump()
{
	if (objCode.f2.AC < limit)
	{	
		clock += 1;
		pc = objCode.f2.AC;
	}
	else
	{
		cout << "!!!Runtime Error: Invalid Address. Program Halted!!!" << endl;
		objCode.i = 49152;
	}
}

void VirtualMachine::jumpl()
{
	if (objCode.f2.AC < limit)
	{	
		clock += 1;
		if (sr & 8)// if less is set
		pc = objCode.f2.AC;
	}	
	else
	{
		cout << "!!!Runtime Error: Invalid Address. Program Halted!!!" << endl;
		objCode.i = 49152;
	}
}  

void VirtualMachine::jumpe()
{
	if (objCode.f2.AC < limit)
	{
		clock += 1;
		if (sr & 4)//if equal is set
		pc = objCode.f2.AC;
	}	
	else
	{
		cout << "Runtime Error: Invalid Address" << endl;
		objCode.i = 49152;
	}
}     

void VirtualMachine::jumpg()
{
	if (objCode.f2.AC < limit)
	{	
		clock += 1;
		if (sr & 2)//if greater is set
		pc = objCode.f2.AC;
	}	
	else
	{
		cout << "Runtime Error: Invalid Address" << endl;
		objCode.i = 49152;
	}
}

void VirtualMachine::call()
{
	clock += 1;
	mem[--sp] = pc;//pushing pc
 	
	for (int i = 0; i < 4; i++)//pushing r[0]~r[3]
		mem[--sp] = r[i];
	
	mem[--sp] = sr;//pushing sp
	
	pc = objCode.f2.AC;//loading pc to jumping address
}      

void VirtualMachine::myReturn()
{
	clock += 1;
	
	sr = mem[sp++];//popping sr

	for (int i = 3; i > -1; i--)//popping r[0]~r[3]
		r[i] = mem[sp++];
		
	pc = mem[sp++];//popping pc
}   

void VirtualMachine::read()
{
	cout << "Now reading from .in file " << endl;
	clock += 28;
	dotIn_file >> r[objCode.f1.RD];//read from .in file and put in r[RD]
}  

void VirtualMachine::write()
{
	clock += 28;
//	if((r[objCode.f1.RD] & 0x8000) > 0)
//	{
//		dotOut_file << (r[objCode.f1.RD] | 0xFFFF0000) << endl;
//	}
//	else	
		if ((r[objCode.f1.RD] & 0x8000) > 0)
		{
			r[objCode.f1.RD] = (r[objCode.f1.RD] | 0xffff0000);
		}

		dotOut_file << "\n#####\tResult: " <<  r[objCode.f1.RD] << "\t######\n" << endl;//write r[RD] in .out file 
		cout << "\n#####\tResult: " <<  r[objCode.f1.RD] << "\t#####\n" << endl;//cout r[RD] in .out file 
}    

void VirtualMachine::halt()
{
	clock += 1;
}

void VirtualMachine::noop()
{
	clock += 1;
}
