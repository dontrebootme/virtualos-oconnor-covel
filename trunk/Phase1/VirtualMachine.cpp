#include "VirtualMachine.h"

void VirtualMachine::setcarry()
{
}
void VirtualMachine::getcarry()
{
}
void VirtualMachine::run(string file)
{
}
void VirtualMachine::VirtualMachine()
{
}
void VirtualMachine::load()
{
	if (objCode.f1.I == 0) //If I = 0
		{
			r[objCode.f2.RD] = mem[objCode.f2.ADDR_CONST];
			clock +=4;
		}
	else // If I = 1
		{
			r[objCode.f2.RD] = objCode.f2.ADDR_CONST;
			clock +=1;
		}
}
void VirtualMachine::store()
{
	mem[objCode.f2.ADDR_CONST] = r[objCode.f2.RD];
	clock += 1;
}
void VirtualMachine::add()
{
}
void VirtualMachine::addc()
{
}
void VirtualMachine::sub()
{
}
void VirtualMachine::subc()
{
}
void VirtualMachine::and()
{
}
void VirtualMachine::xor()
{
}
void VirtualMachine::compl()
{
}
void VirtualMachine::shl()
{
}
void VirtualMachine::shla()
{
}
void VirtualMachine::shr()
{
}
void VirtualMachine::shra()
{
}
void VirtualMachine::compr()
{
}
void VirtualMachine::getstat()
{
	clock += 1;
	r[objCode.f1.RD] = sr;
}
void VirtualMachine::putstat()
{
	clock += 1;
	r[objCode.f1.RD];
}
void VirtualMachine::jump()
{
	clock += 1;
	if (sr & 4)
		pc = objCode.f2.ADDR_CONST;
}
void VirtualMachine::jumpl()
{
	clock += 1;
	if (sr & 2)
		pc = objCode.f2.ADDR_CONST;
}
void VirtualMachine::jumpe()
{
	clock += 1;
	if (sr & 4)
		pc = objCode.f2.ADDR_CONST;
}
void VirtualMachine::jumpg()
{
	clock += 1;
	if (sr & 2)
		pc = objCode.f2.ADDR_CONST;
}
void VirtualMachine::call()
{
}
void VirtualMachine::return()
{
}
void VirtualMachine::read()
{
}
void VirtualMachine::write()
{
}
void VirtualMachine::halt()
{
        clock += 1;
}
void VirtualMachine::noop()
{
        clock += 1;
}
