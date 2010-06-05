/************************************************
 * os.cpp
 * Patrick O'Connor, Timothy Covel
 * Apr/21/2010
 * to compile: g++ -o os os.cpp Assembler.cpp VirtualMachine.cpp
 * to run: ./os prog.s
 * Depends on Assembler.h Assembler.cpp VirtualMachine.h VirtualMachine.cpp
 *
 * Main program for running the assembler and virtual machine for
 * the virtual operating system project, Phase 1
*************************************************/
#include "os.h"

	                                        
OS::OS(){
	invertPageTable = vector<InvertedPage*>(16);
	
	for(int i = 0; i < invertPageTable.size(); i++)
	{
		invertPageTable[i] = new InvertedPage;
		invertPageTable[i]->frameOwner = "";
		invertPageTable[i]->page = 0;
	}

        AssembleProgs();
	idleTime = idleCounter = 0;
	FIFO = true;
	LRU = false;
	}

void OS::AssembleProgs(){

	system("ls *.s > progs");
	fstream progList;
	progList.open("progs",ios::in);
	
	string program;
	
	while(progList >> program){
		as.assemble(program);
	        PCB * p = new PCB;
	        p -> pName = program;
	        readyQ.push(p);
	        pcb.push_back(p);
	}
	vm.loadMemory(pcb);
	for(int i = 0; i < pcb.size(); i++)
	{
        	invertPageTable[pcb.front()->pc/16]->frameOwner = pcb.front()->pName;
        	invertPageTable[pcb.front()->pc/16]->page = 0;
        	FIFORef.push(pcb.front()->pc/16);
        	pcb.push_back(pcb.front());
        	pcb.pop_front();
	}


	progList.close();
	running=readyQ.front();
	readyQ.pop();
}	

void OS::checkKillFrames(stack<int> kill_frames)
{
        while(!kill_frames.empty())
        {
                int k_frame = kill_frames.top();

                kill_frames.pop();
                string owner;
                owner =  invertPageTable[k_frame-1]->frameOwner;

                invertPageTable[k_frame]->frameOwner = "";
                for(int i = 0; i < pcb.size(); i++)
                {
                        if(pcb.front()->pName == owner)
                                break;
                        else
                        {
                                pcb.push_back(pcb.front());
                                pcb.pop_front();
                        }
                }

                for(int j = 0; j < pcb.front()->pageTable.size();j++)
                {
                        if((pcb.front()->pageTable[j])->frame == k_frame)
                                {
                                        (pcb.front()->pageTable[j])->validBit = 0;
                                        break;
                                }
                }
        }
}

int OS::contextSwitch()
{
	switch(vm.vm_sr()){

      	case 0://Time slice occured
        	readyQ.push(running);
                running -> readyTimeStamp = vm.clock;
                break;
      	case 1://Halt instr occured
        	terminateJob.push_back(running);
                if(terminateJob.size() == pcb.size())
                goto done;
                break;
      	case 2://out-bound
                (running->pcbOutFile)  << "An out-of-bound reference was made." << endl;
                terminateJob.push_back(running);
                break;
        case 3://stack overflow occured
                (running->pcbOutFile) << "Stack overflow occured." << endl;
                terminateJob.push_back(running);
         	break;
      	case 4://Stack underflow occured
                (running->pcbOutFile) << "Stack underflow occured." << endl << "PROGRAM TERMINATED!" << endl;
                terminateJob.push_back(running);
        	break;
      	case 6://I/O operation occured
        	running->IO_clock = vm.clock + 26;
                waitQ.push(running);
                running->waitTimeStamp = vm.clock;
		running->triger = false;
                break;
      	case 7://I/O operation occured
        	running->IO_clock = vm.clock + 26;
                waitQ.push(running);
                running->waitTimeStamp = vm.clock;
		running->triger = false;
                break;
	case 8://page fault  
                running -> IO_clock = vm.clock + 40;
                waitQ.push(running);
                running -> waitTimeStamp = vm.clock;
                running->triger = true;
                ++(running->pf);
                break;
      	done:
                printInfo();
                closeStreams();
                return -1;
      		
	default:
         	cout << "Invalid vm return status " << vm.vm_sr() << endl;
         	break;
	}
	vm.clock += 5;
	running =0;
}

void OS::closeStreams()
{
        list<PCB *>::iterator itr = terminateJob.begin();

        for(; itr != terminateJob.end(); itr++)
                {
                        (*itr)->pcbInFile.close();
                        (*itr)->pcbObjectFile.close();
                        (*itr)->pcbOutFile.close();
                        (*itr)->pcbStateFile.close();
                }

}

int OS::getEmptyFrame()
{
        for(int i = 0; i < invertPageTable.size(); i++)
        {
                if(invertPageTable[i]->frameOwner == "")
                        return i;
        }

        return -1;
}

bool OS::checkPage(int page, PCB *p)
{
        string file = p->pName;
        file = file.substr(0,file.length()-2)+".o";

        fstream strea;
        strea.open(file.c_str(), ios::in);

        int counter, temp;

        for(counter = 0;strea >> temp; counter++);

        counter = ceil(static_cast<double>(counter)/16.0);
        if(counter >= page)
                return true;
        else
                return false;
}

void OS::loadPage(PCB * p)
{
        int empty_frame = getEmptyFrame();

        int temp, limit;

        if(!checkPage(p->pageRequest, p))
        {
                cout << "PAGE NOT IN RANGE" << endl;
                exit(1);
        }

        if(empty_frame == -1)
        {
                int time = LONG_MAX;

                if(FIFO)
                {
                        empty_frame = FIFORef.front();
                        FIFORef.push(FIFORef.front());
                        FIFORef.pop();
                }

                if(LRU)
                {
                        for(int i = 0; i < 16; i++)
                        {
                                if(vm.frameTimeStamps[i] < time)
                                {
                                        empty_frame = i;
                                        time = vm.frameTimeStamps[i];
                                }
                        }
                }
                int j =0;
                PCB* replacingPCB;

                for(int i = 0; i < pcb.size(); i++)
                {
                        if(invertPageTable[empty_frame]->frameOwner == pcb.front()->pName)
                        {
                                for(; j < 16; j++)
                                {
                                        if(pcb.front()->pageTable[j]->frame == empty_frame)
                                        {
                                                pcb.front()->pageTable[j]->validBit = 0;
                                                replacingPCB = pcb.front();
                                                break;
                                        }
                                }
                        }
                        pcb.push_back(pcb.front());
                        pcb.pop_front();
                }

        }

        Page * pp = new Page;

        pp->validBit = 1;
        pp->frame = empty_frame;
        FIFORef.push(empty_frame);

        invertPageTable[empty_frame]->frameOwner = p->pName;
        invertPageTable[empty_frame]->page = p->pageRequest;

        string file = p->pName;
        file = file.substr(0,file.length()-2)+".o";

        (p->pcbObjectFile).open(file.c_str(),ios::in);

        for(int i = 0; i < p->pageRequest*16; i++)
                p->pcbObjectFile >> temp;

        for(limit=0; limit < 16 && p->pcbObjectFile >> temp; limit++)
                vm.mem[16 * empty_frame + limit] = temp;
                //vm.mem.push_back(temp);

        pp->limit = limit;
        p->limit = limit;

        if(limit < 16)
        {
                for(limit; limit < 16; limit++);
                        //vm.mem[16*empty_frame+limit] = 0;
                        //vm.mem.push_back(-1);
        }

        if(p->adPC)
                p->pc = 16*empty_frame;// + p->displ;

        p->adPC = false;

        pp->base = 16*empty_frame;
        p->base = 16*empty_frame;
        p->pageTable[p->pageRequest] = pp;

        p->triger = false;

        (p->pcbObjectFile).close();
}

void OS::checkWaiting(){

        //poping all ready items into readyQ
   while( !waitQ.empty() && waitQ.front() -> IO_clock < vm.clock){
         readyQ.push(waitQ.front());

                 if(waitQ.front()->triger)
                                loadPage(waitQ.front());

                 waitQ.front() -> readyTimeStamp = vm.clock ;
                 waitQ.front() -> ioTime += (vm.clock - (waitQ.front() -> waitTimeStamp));
       waitQ.pop();
        }

}

void OS::nextJob(){

        //assigning the next task to run
        if(!readyQ.empty()){
                running = readyQ.front();
                readyQ.front() -> waitTime += (vm.clock - (running -> readyTimeStamp));
                readyQ.pop();
        }else running =0;

}

void OS::idle(){

                for(idleCounter=0; waitQ.front() -> IO_clock >= vm.clock; vm.clock++, idleCounter++);
        idleTime += idleCounter;

}


void OS::run(){

	bool first_run = false;
	int temp = 0;
	while( true ){
		checkWaiting();
        	if(!first_run){
                	temp = vm.clock-5;
                	vm.run(running);
                	running->CPU_time += (vm.clock-temp-5);
        	}
		else{
                	temp = vm.clock;
                	vm.run(running);
                	running->CPU_time += (vm.clock-temp);
        	}
		if(contextSwitch() == -1)
			goto exit;
		
		while(readyQ.empty() && running ==0)
		{
			idle();
			checkWaiting();
		}

		nextJob();
		first_run = true;
	}	
	exit:;
}

void OS::printInfo(){

	list<PCB *>::iterator itr;
	itr = terminateJob.begin();

	double ioTime=0;
	for(;itr != terminateJob.end(); itr++)
	{
        	(*itr)->pcbOutFile << "\nCPU Time: " << (*itr)->CPU_time << endl;
        	(*itr)->pcbOutFile << "Largest stack size: " << (*itr)->largestStackSize << endl;
        	(*itr)->pcbOutFile << "I/O Time in waitQ: " << (*itr)->ioTime << endl;
        	(*itr)->pcbOutFile << "Waiting Time in readyQ: " << (*itr)->waitTime << endl;
        	(*itr)->pcbOutFile << "Turn around time: " << (*itr)->tTime <<endl;
        	(*itr)->pcbOutFile << "Number of page fault: " << (*itr)->pf << endl;
        	(*itr)->pcbOutFile << "Hit ratio: " << (*itr)->HR << endl;
        	ioTime += (*itr)->ioTime;
	}

	itr = terminateJob.begin();
	for(;itr != terminateJob.end(); itr++)
	{
        (*itr)->pcbOutFile << "\nCPU idled for: " << idleTime << " ticks." << endl;
        (*itr)->pcbOutFile << "\nSystem Information: " << endl;
        (*itr)->pcbOutFile  << "\tCPU Utilization: " << setprecision(4)
                 << static_cast<double>((vm.clock-idleTime))/static_cast<double>(vm.clock)*100 << "%"<<endl;
        (*itr)->pcbOutFile  << "\tThroughput: " << (static_cast<double>(pcb.size())/(static_cast<double>(vm.clock)))*1000<< " seconds" << endl;
	}
}


int main(){
	OS os;
	os.run();
	return 0;
}
