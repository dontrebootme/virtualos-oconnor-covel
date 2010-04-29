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


main(){
OS os;
//os.run();
}

/*
int main(int argc, char *argv[])
{
	system("ls *.s > progs");
	
	if(argc == 1 || argc >= 3)
		cout << "Missing argument or too many arguments.\n";
	else
	{
		string file = argv[1];
		if(*(file.end()-2) == '.' && *(file.end()-1) == 's')
			{
				Assembler as;
				VirtualMachine vm;
				as.assemble(file);
				vm.run(file);
			}
		else
			cout << "Improper suffix used, please rename your assembly program with proper suffix.\n";
	}

}
*/
