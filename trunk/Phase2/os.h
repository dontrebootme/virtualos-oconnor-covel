#ifndef OS_H
#define OS_H

#include "Assembler.h"
#include "VirtualMachine.h"
#include <iomanip>
#include <list>
#include <queue>

class OS{

public:
        OS();
        void run();

private:
        void AssembleProgs();
        Assembler as;
        VirtualMachine vm;
        list<PCB *> pcb, terminateJob;
        queue<PCB *> readyQ, waitQ, runQ;
        PCB * running;
};

#endif
