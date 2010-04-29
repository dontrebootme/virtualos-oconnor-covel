#ifndef OS_H
#define OS_H

#include "Assembler.h"
#include "VirtualMachine.h"
#include <iomanip>
#include <list>
#include <queue>

struct PCB{
        vector<int> r;
        int pc, sr, sp, base, limit, IO_clock;
        int CPU_time, largest_stack_size, ta_time, io_time, waiting_time;
        string pName;

        ifstream pcbfin;//.in
        ofstream pcbfout;//.out
        fstream pcbfst;//.st
};


class OS{

public:
        OS();
        void run();

private:
        void AssembleProgs();
        Assembler as;
        VirtualMachine vm;
        list<PCB *> pcb;
        queue<PCB *> readyQ, waitQ;
        PCB * running;
};

#endif
