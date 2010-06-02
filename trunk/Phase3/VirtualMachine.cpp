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
	
	if(r[objCode.f1.RD] & 0x10000) //check if 17th bit of RD is 1
		sr = sr | 1; //If it isnt set, set it
	else 
		sr = sr & 0x1E; //else Carry is 0
}

bool VirtualMachine::getCarry()	//return 1 if Carry flag is set
{											
	if (sr & 1) // check if Carry flag is 1
		return true; // Carry Flag is Set
	else 	   //otherwise
		return false; //Carry Flag is not Set
}


void VirtualMachine::init(PCB *p)
{
                p -> r.reserve(4);

                for(int i = 0; i < 4; i++)
                        p->r[i] = 0;

                p->pageTable.reserve(16);

                for(int i=0; i < 16; i++)
                {
                        p->pageTable[i] = new Page;
                        p->pageTable[i]->validBit = 0;
                }

                p->sp=256;
                p->sr=0;
                p->CPU_time=0;
                p->largestStackSize=0;
                p->tTime=0;
                p->ioTime=0;
                p->IO_clock=0;
                p->readyTimeStamp=0;
                p->waitTimeStamp=0;
                p->waitTime=0;
                p->pageRequest=0;
                p->pageCounter=0;
                p->HR=0;
                p->pf=0;

}


void VirtualMachine::loadMemory(list<PCB *> &pcb)
{
        int temp;
        string file;

        list<PCB *>::iterator PCBit;

        system("rm -f VirtualMachine.o assembler.o os.o");
        system("ls *.o > objCodeFile");
        ifstream objCodeFile;
        ifstream objSubFile;
        objCodeFile.open("objCodeFile",ios::in);

        PCBit = pcb.begin();
        for(;objCodeFile >> file;PCBit++){
                Page * pp = new Page;

                (*PCBit)->pcbObjectFile.open(file.c_str(), ios::in);

                init(*PCBit);

                if(PCBit == pcb.begin())
                {
                        (*PCBit) -> pc = 0;
                        (*PCBit) -> base= 0;
                }
                else
                {
                        (*PCBit) -> pc = counter;
                        (*PCBit) -> base = counter;
                }

                pp->frame = counter/16;

                for(limit=0; limit < 16 && (*PCBit)->pcbObjectFile >> temp; counter++, limit++)
                        mem[counter] = temp; 

                pp->validBit = 1;

                if(limit <      16)
                        for(int i=limit; i < 16; counter++, i++)
                                mem[counter] = -1;

                (*PCBit) -> limit = limit;

                (*PCBit)->pcbObjectFile.close();

                file = file.substr(0,file.length()-2) + ".in";
                ((*PCBit)->pcbInFile).open(file.c_str(),ios::in);

                file = file.substr(0,file.length()-3) + ".out";
                ((*PCBit) -> pcbOutFile).open(file.c_str(),ios::out);

                (*PCBit)->pageTable[0] = pp;

        }
        objCodeFile.close();
}

void VirtualMachine::saveState(PCB * p)
{
        for(int i=0; i < 4; i++)//saving registers
                p->r[i] = r[i];

        p->sr = sr;
        p->sp= sp;
        p->pc = pc;
        p->base = base;
        p->limit = limit;

        string file = (p->pName);
        file = file.substr(0,(file.length()-2));
        file += ".st";


        (p-> pcbStateFile).open(file.c_str(), ios::out);

        if(sp != 256)
        {
                for(int i=sp; i != 256; i++)
                        (p -> pcbStateFile) << mem[i] << endl;
        }

        (p-> pcbStateFile).close();
}

void VirtualMachine::loadState(PCB * p)
{
        int temp;

	for(int i = 0; i < 16; i++)
		TLB[i] = p->pageTable[i];

        for(int i = 0; i < 4; i++)
                r[i] = p->r[i];


        sr = p->sr;
        sp = p->sp;
        pc = p->pc;
        base = pc;
        limit = pc + 16;

        string file = (p->pName);
        file = file.substr(0,(file.length()-2));
        file += ".st";

        (p->pcbStateFile).open(file.c_str(),ios::in);

        if(sp != 256)
        {
                for(int i = sp; (p->pcbStateFile) >> temp;i++)
                        mem[i] = temp;
        }
        (p->pcbStateFile).close();
	frameTimeStamps[pc/16] = clock;
}

void VirtualMachine::CheckAddr(int addr)
{
        if(TLB[addr/16]->validBit == 1)
        {
                pc = ((TLB[addr/16]->frame)*16) + (addr % 16);

                TLB[addr/16]->base = TLB[addr/16]->frame*16;

                base = TLB[addr/16]->base;
                limit = base + 16;
        }
        else
        {
                current->adPC = false;
                current->pageRequest = addr/16;
                pageFault = true;
                pc--;
        }
}


void VirtualMachine::run(PCB * p)
{
        int timeUp;

        current = p;
	
        loadState(p);

        timeUp = clock+timeSlice;
        for(;;)//entering infinit loop of fetch-execute cycle
        {
                ir = mem[pc];
                pc++;
                objCode.i = ir;
                (this->*funcMap[objCode.f1.OP])();

                sr = sr & 0x1F;//clearing vm_return status

                if(sp < (counter + 6))//stack overflow
                {
                        sr = sr | 0x60;//setting overflow flag
                        //cout << "Stack Overflow: " << sr << endl;
			saveState(p);
                        break;
                }

                if(sp > 256)//stack underflow
                {
                        sr = sr | 0x80;//underflow flag
                        //cout << "Stack Underflow: " << sr << endl;
                        saveState(p);
                        break;
                }


                if((objCode.f1.OP == 0 && objCode.f1.I == 0) || (objCode.f1.OP == 1))
                {
                        if(clock >= (timeUp+3))
                        {
				//cout << "timeUp: " << timeUp << " clock: " << clock << endl; 
                                saveState(p);
				//cout << "breaking 1" << endl;
                                break;
                        }
                }
                if((objCode.f1.OP == 0 && objCode.f1.I == 0) ||//load RD AC
                        (objCode.f1.OP == 1) || //store RD AC
                        (objCode.f1.OP == 16) || //jump AC
                        (objCode.f1.OP == 17) || //jumpl AC
                        (objCode.f1.OP == 18) || //jumpe AC
                        (objCode.f1.OP == 19) || //jumpg AC
                        (objCode.f1.OP == 20)) //call AC
                {
                        if(!((objCode.f2.AC+base < base + limit)
                         && (objCode.f2.AC+base >= base) && !(objCode.f2.AC <= 0)))
                        {
                                //sr = sr | 0x60; // out-bound was made 
                                sr = sr | 0x40;
				saveState(p);
				//cout << "breaking 3" << endl;
                                break;
                        }
                }

		if(objCode.f1.OP == 22){
                	//cout << "IO Operation, SR before: " << sr << endl;
		        sr = sr | 0xF0;//io operation
			//cout << "IO Operation: " << sr << endl;
			checkRange(p);
                        saveState(p);
                        break;
		}

                if(objCode.f1.OP == 23){
                	//cout << "IO Operation, SR before: " << sr << endl;
		        sr = sr | 0xD0;//io operation
			//cout << "IO Operation: " << sr << endl;
			checkRange(p);
                        saveState(p);
                        break;
                }

                if(objCode.i == 49152){
                        sr = sr | 0x20;//halt instr
                        p->tTime = clock;
			//cout << "halt instruction: " << sr << endl;
                        saveState(p);
                        break;
                }

		if(pageFault){
			sr = sr | 0xE0;
			pageFault = false;
			saveState(p);
			break;
		}
                if(pc >= limit)
                {
                        ++(p->pageCounter);

                        if(getCurrentPage() < 0)//page wasnt found
                                        goto skip1;

                        if(TLB[p->pageCounter]->validBit == 1)
                        {
                                pc = (TLB[(p->pageCounter)]->frame * 16) + pc % 16;
                                base = pc/16;
                                limit = base + 16;
                                ++(p->HR);
                                clock += 4;
                                frameTimeStamps[pc/16] = clock;
                                goto skip2;
                        }
                        skip1:;
                        sr = sr | 0xE0;
                        saveState(p);
                        p->pageRequest = p->pageCounter;
                        p->adPC = true;
                        break;
                }
                skip2:;

        }
}

void VirtualMachine::checkRange(PCB* p)
{
        if(pc >= limit)
                {
                        ++(p->pageCounter);

                        if(getCurrentPage() < 0)//page wasnt found
                                        goto skip1;

                        if(TLB[p->pageCounter]->validBit == 1)
                        {
                                pc = (TLB[(p->pageCounter)]->frame * 16) + pc % 16;
                                base = pc/16;
                                limit = base + 16;
                                ++(p->HR);
                                clock += 4;
                                frameTimeStamps[pc/16] = clock;
                                goto skip2;
                        }

                        skip1:;
                        sr = sr | 0xE0;
                        saveState(p);
                        p->pageRequest = p->pageCounter;
                        p->adPC = true;

                }
                skip2:;
}


VirtualMachine::VirtualMachine()
{
	//initializing data types
	counter=0;
	timeSlice = 15;
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

int VirtualMachine::getFrame(int page)
{
        if(TLB[page]->validBit == 0)
                return -1;
        else
                return TLB[page]->frame;
}

int VirtualMachine::getCurrentPage()
{
        int currentFrame = (pc)/16;

        for(int i = 0; i < 16; i++)
        {
                if(TLB[i]->frame == currentFrame)
                        return i;
        }

        return -1;
}


void VirtualMachine::load() 
{
	if ( objCode.f1.I == 0 ) 
		{
			int gf = getFrame(objCode.f2.AC/16);

			if( gf == -1)
			{
				current -> pageRequest = objCode.f2.AC/16;
				current -> adPC = false;
				pageFault = true;
				pc --;
			}
			else
			{
				r[objCode.f2.RD] = mem[(gf*16)+(objCode.f2.AC%16)];
				if(objCode.f2.AC > limit)
					{
						++(current->HR);
					}
			clock += 4;
			}
		}
	else 
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
	int gf = getFrame(objCode.f2.AC/16);

	if (gf == -1)
	{
		current->adPC = false;
		current->pageRequest = objCode.f2.AC/16;
		pageFault = true;
		pc --;
	}
	else
	{
		if(objCode.f2.AC > limit)
		{
			++(current->HR);
		}	
	mem[(gf*16)+objCode.f2.AC%16] = r[objCode.f2.RD];
	}
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
		CheckAddr(objCode.f2.AC);
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
			CheckAddr(objCode.f2.AC);
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
			CheckAddr(objCode.f2.AC);
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
			CheckAddr(objCode.f2.AC);
	}	
	else
	{
		cout << "Runtime Error: Invalid Address" << endl;
		objCode.i = 49152;
	}
}

void VirtualMachine::call()
{
	clock += 4;
	current->killFrames.push(sp/16);

	if(TLB[objCode.f2.AC/16]->validBit == 0)
	{
		current->pageRequest = objCode.f2.AC/16;
		current->adPC = false;
		pc--;
		pageFault = true;
		return;
	}
	mem[--sp] = getCurrentPage();
	mem[--sp] = pc%16;//pushing pc
 	
	for (int i = 0; i < 4; i++)//pushing r[0]~r[3]
		mem[--sp] = r[i];
	
	mem[--sp] = sr;//pushing sp
	
	CheckAddr(objCode.f2.AC);
	current->largestStackSize = (256-sp);
}      

void VirtualMachine::myReturn()
{
	clock += 4;
	int i;

	current->killFrames.pop();
	
	if(TLB[mem[sp+6]]->validBit == 0)
	{
		current->pageRequest = mem[sp+6];
		current->adPC = false;
		pc--;
		pageFault = true;
		return;
	}
	
	sr = mem[sp++];//popping sr

	for (int i = 3; i > -1; i--)//popping r[0]~r[3]
		r[i] = mem[sp++];
	
	i = mem[sp++];
	i += (getFrame(mem[sp++]) * 16);
	
	pc = i;
}   

void VirtualMachine::read()
{
	//cout << "Now reading from .in file " << endl;
	clock += 28;
	(current->pcbInFile) >> r[objCode.f1.RD];
	//dotIn_file >> r[objCode.f1.RD];//read from .in file and put in r[RD]
}  

void VirtualMachine::write()
{
	clock += 28;
		
		if ((r[objCode.f1.RD] & 0x8000) > 0)
		{
			r[objCode.f1.RD] = (r[objCode.f1.RD] | 0xffff0000);
		}
	(current->pcbOutFile) << r[objCode.f1.RD] << endl;
		//dotOut_file << "\n#####\tResult: " <<  r[objCode.f1.RD] << "\t######\n" << endl;//write r[RD] in .out file 
		//cout << "\n#####\tResult: " <<  r[objCode.f1.RD] << "\t#####\n" << endl;//cout r[RD] in .out file 
}    

void VirtualMachine::halt()
{
	clock += 1;
}

void VirtualMachine::noop()
{
	clock += 1;
}
