#include "VirtualMachine.h"

void VirtualMachine::setCarry()			
{
	
	if(r[objCode.f1.RD] & 0x00010000) 
		sr = sr | 1; //set carry flag if 17th bit of RD is 1
	else 
		sr = sr & 0x0000001E; //else carry is 0
}

bool VirtualMachine::getCarry()	//return 1 if carry flag is set
{											
	if (sr & 1) 
		return 1;							
	else 
		return 0;
}

void VirtualMachine::run(string file)
{
	int temp;
	wfile.assign(file,0,file.length()-2);
	rfile=wfile;
	rfile+=".in";
	wfile+=".out";
	
	file.erase(file.end()-2,file.end());
	file += ".o";	
	
	dotO_file.open(file.c_str(), ios::in);//open .o file for reading
	dotIn_file.open(rfile.c_str(),ios::in);//open .in file for reading
	dotOut_file.open(wfile.c_str(), ios::out);//opening .out file for writing

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
			r[objCode.f2.RD] = objCode.f2.AC;
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
		setcarry();
     }
	else {
		
		if(objCode.f2.RD >= 0 && objCode.f2.AC >=0 && ((objCode.f2.RD + objCode.f2.AC) < 0))
			sr = sr | 0x00000010;
		else if(objCode.f2.RD < 0 && objCode.f2.AC < 0 && ((objCode.f2.RD + objCode.f2.AC) >= 0))
			sr = sr | 0x00000010;
		
		r[objCode.f2.RD] += objCode.f2.AC;
		setcarry();
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
		
		if ( getcarry() ) 
			r[objCode.f1.RD] += r[objCode.f1.RS] + 1;
		else//Carry is not set
			r[objCode.f1.RD] += r[objCode.f1.RS];
		
		setcarry();
    }
	else //I=1
	{
		if(objCode.f2.RD >= 0 && objCode.f2.AC >=0 && ((objCode.f2.RD + objCode.f2.AC) < 0))
			sr = sr | 0x00000010;//seting overflow flag
		else if(objCode.f2.RD < 0 && objCode.f2.AC < 0 && ((objCode.f2.RD + objCode.f2.AC) >= 0))
			sr = sr | 0x00000010;
		
		if ( getcarry() ) 
			r[objCode.f2.RD] += objCode.f2.AC + 1;
		else//carry is not set
			r[objCode.f2.RD] += objCode.f2.AC;
		
		setcarry();
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
		setcarry();
   }
	else//I=1
	{	 
		if(objCode.f2.RD >= 0 && objCode.f2.AC >=0 && ((objCode.f2.RD + objCode.f2.AC) < 0))
			sr = sr | 0x00000010;//seting overflow flag
		else if(objCode.f2.RD < 0 && objCode.f2.AC < 0 && ((objCode.f2.RD + objCode.f2.AC) >= 0))
			sr = sr | 0x00000010;
		
		r[objCode.f2.RD] -= objCode.f2.AC;
		setcarry();
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
		
		if ( getcarry() ) 
			r[objCode.f1.RD] -= r[objCode.f1.RS] - 1;
		else//no carry
			r[objCode.f1.RD] -= r[objCode.f1.RS];

		setcarry();
    }
	else //I=1
	{
      if(objCode.f2.RD >= 0 && objCode.f2.AC >=0 && ((objCode.f2.RD + objCode.f2.AC) < 0))
         sr = sr | 0x00000010;//seting overflow flag
      else if(objCode.f2.RD < 0 && objCode.f2.AC < 0 && ((objCode.f2.RD + objCode.f2.AC) >= 0))
         sr = sr | 0x00000010;
	
		if ( getcarry() ) 
			r[objCode.f2.RD] -= objCode.f2.AC - 1;
		else//no carry
			r[objCode.f2.RD] -= objCode.f2.AC;

		setcarry();
	}
}	
	
void VirtualMachine::myAnd()
{
	clock += 1;
	if ( objCode.f1.I == 0 ) //I=0
		r[objCode.f1.RD] = r[objCode.f1.RD] & r[objCode.f1.RS];
	else //I=1
		r[objCode.f2.RD] = r[objCode.f2.RD] & objCode.f2.AC;
}	


void VirtualMachine::myXor()
{
	clock += 1;
	if ( objCode.f1.I == 0 ) 
		r[objCode.f1.RD] = r[objCode.f1.RD] ^ r[objCode.f1.RS];
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
	setcarry();
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

	setcarry();
}    

void VirtualMachine::shr()
{
	clock += 1;
	if ( r[objCode.f1.RD] & 1)//check if 1st bit is 1
		sr = sr | 1;//set carry if 1st bit is 1

	r[objCode.f1.RD] = r[objCode.f1.RD] >> 1;
}       

void VirtualMachine::shra()
{
	clock += 1;
	if(r[objCode.f1.RD] & 1)// check if 1st bit is 1
		sr = sr | 1;//set carry if 1st bit is 1
			
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

void VirtualMachine::getstat()
{
	clock += 1;
	r[objCode.f1.RD] = sr;
}

void VirtualMachine::putstat()
{
	clock += 1;
	sr = r[objCode.f1.RD];
}   

void VirtualMachine::jump()
{
	clock += 1;
	pc = objCode.f2.AC;
}

void VirtualMachine::jumpl()
{
	clock += 1;
	if (sr & 8)// if less is set
		pc = objCode.f2.AC;
}  

void VirtualMachine::jumpe()
{
	clock += 1;
	if (sr & 4)//if equal is set
		pc = objCode.f2.AC;
}     

void VirtualMachine::jumpg()
{
	clock += 1;
	if (sr & 2)//if greater is set
		pc = objCode.f2.AC;
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
	clock += 28;
	dotIn_file >> r[objCode.f1.RD];//read from .in file and put in r[RD]
}  

void VirtualMachine::write()
{
	clock += 28;
	dotOut_file << r[objCode.f1.RD] << endl;//write r[RD] in .out file 
}    

void VirtualMachine::halt()
{
	clock += 1;
}

void VirtualMachine::noop()
{
	clock += 1;
}
