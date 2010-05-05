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
#include "Assembler.h"
#include "VirtualMachine.h"
#include "os.h"

	                                        
OS::OS(){
        AssembleProgs();
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
progList.close();
}	

void OS::run(){

int idle_counter=0, idle_time=0;

//setting first job
running=readyQ.front();
readyQ.pop();

bool first_run = false;
int temp = 0;
while( true ){

        if(!first_run){
                temp = vm.clock-5;
                vm.run(running);
                running->CPU_time += (vm.clock-temp-5);
        }else{
                temp = vm.clock;
                vm.run(running);
                running->CPU_time += (vm.clock-temp);
        }

   	if( !waitQ.empty() && (*waitQ.front()).IO_clock <= vm.clock){
                readyQ.push(waitQ.front());

                temp = vm.clock - (*waitQ.front()).io_time;
                (*waitQ.front()).io_time += temp;

                waitQ.pop();
        }

        int wait_stamp=0,ready_stamp=0;
        switch(vm.vm_sr()){

      	case 0://Time slice occured
                        readyQ.push(running);
                        ready_stamp = vm.clock;
                        break;
      	case 1://I/O operation occured
         		running->IO_clock = vm.clock + 26;
                        waitQ.push(running);
                        wait_stamp = vm.clock;
                        break;
      	case 2://Halt instr occured
         		terminateJob.push_back(running);
                        if(terminateJob.size() == pcb.size())
                        	goto done;
                        break;
      	case 3://out-bound
                        (running->pcbOutFile)
                            << "An out-of-bound reference was made." << endl;
                        terminateJob.push_back(running);
                        break;
        case 4://stack overflow occured
                        (running->pcbOutFile) << "Stack overflow occured." << endl;
                        terminateJob.push_back(running);
         		break;
      	case 5://Stack underflow occured
                        (running->pcbOutFile) << "Stack underflow occured." << endl << "PROGRAM TERMINATED!" << endl;
                        terminateJob.push_back(running);
        		break;
      	default:
         	cout << "Invalid vm return status " << vm.vm_sr() << endl;
         	break;
   }//select

        //null because job came back and was placed in right queue
        running = 0;

        //idle case
        if(readyQ.empty() && running == 0)
        {
                for(;(*waitQ.front()).IO_clock >= vm.clock; vm.clock++, idle_counter++);
                idle_time += idle_counter;
                idle_counter = 0;
                readyQ.push(waitQ.front());
                temp = vm.clock - wait_stamp;
                (*waitQ.front()).io_time += temp;

                waitQ.pop();
        }
        if(!readyQ.empty()){
                running = readyQ.front();
                temp = vm.clock - ready_stamp;
                (*readyQ.front()).waiting_time += temp;
                readyQ.pop();
        }
        else running = 0;

        vm.clock += 5;//charging timer for context switch
        first_run = true;
}//while */
done:;

list<PCB *>::iterator itr;
itr = terminateJob.begin();

int throughPut=0;
for(;itr != terminateJob.end(); itr++)
{
        (*itr)->pcbOutFile << "\nCPU Time: " << (*itr)->CPU_time << endl;
        (*itr)->pcbOutFile << "Largest stack size: " << (*itr)->largest_stack_size << endl;
        (*itr)->pcbOutFile << "I/O Time in waitQ: " << (*itr)->io_time << endl;
        (*itr)->pcbOutFile << "Waiting Time in readyQ: " << (*itr)->waiting_time << endl;
        (*itr)->pcbOutFile << "Turn around time: "
                                        << static_cast<double>((*itr)->ta_time)/1000.0 << " second(s)\n";
        throughPut += (*itr)->ta_time;
}
cout << "\nCPU idled for: " << idle_time << " ticks.\n";
cout << "\nSystem Information: \n";
cout << "\tCPU Utilization: " << setprecision(4)
         << static_cast<double>((vm.clock-idle_time))/static_cast<double>(vm.clock)*100 << "%\n";
cout << "\tThroughput: " << throughPut/1000.0 << " second(s)\n\n";
}


int main(){
OS os;
os.run();
return 0;
}
