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
}
void VirtualMachine::putstat()
{
}
void VirtualMachine::jump()
{
}
void VirtualMachine::jumpl()
{
}
void VirtualMachine::jumpe()
{
}
void VirtualMachine::jumpg()
{
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
}
void VirtualMachine::noop()
{
}
